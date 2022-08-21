/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <vector>
#include "cGZMessageServer2.h"
#include "cIGZFramework.h"
#include "cIGZMessage2.h"
#include "cIGZMessageTarget2.h"
#include "cRZMessage2Standard.h"
#include "RZStatics.h"

cGZMessageServer2::GeneralMessageTargetInfo::GeneralMessageTargetInfo(GeneralMessageTargetInfo const& other) :
	target(other.target),
	msg(other.msg)
{
}

cGZMessageServer2::cGZMessageServer2() :
	cRZSystemService(RZSRVID_cGZMessageServer2, 5000000),
	alwaysClearQueueOnTick(true)
{
}

cGZMessageServer2::~cGZMessageServer2()
{
	// TODO
}

bool cGZMessageServer2::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessageServer2)
	{
		*outPtr = static_cast<cIGZMessageServer2*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZMessageServer2::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZMessageServer2::Release()
{
	return cRZSystemService::Release();
}

bool cGZMessageServer2::Init()
{
	return RZGetFramework()->AddToTick(this);
}

bool cGZMessageServer2::Shutdown()
{
	return RZGetFramework()->RemoveFromTick(this);
}

bool cGZMessageServer2::MessageSend(cIGZMessage2* msg)
{
	std::vector<cRZAutoRefCount<cIGZMessageTarget2> > targetsToNotify;

	notificationLock.Lock();
	DoDelayedNotificationAdditionsAndRemovalsImmediately();

	++pendingNotificationOps;
	GZGUID msgType = msg->GetType();

	while (true)
	{
		MessageTargetMap::iterator targetMapIt = notificationTargets.find(msgType);
		if (targetMapIt != notificationTargets.end())
		{
			const MessageTargetList* targetList = targetMapIt->second;

			MessageTargetList::const_iterator listIt = targetList->begin();

			if (listIt != targetList->end())
			{
				cRZAutoRefCount<cIGZMessageTarget2> emptyTarget;
				while (listIt != targetList->end())
				{
					cIGZMessageTarget2* realTarget = *listIt;
					targetsToNotify.push_back(emptyTarget);
					targetsToNotify.back() = realTarget; // FUTURE: why does the shipped code look like this?
					listIt++;
				}
			}
		}

		if (msgType == 0)
		{
			break;
		}

		// Let listeners use GZGUID = 0 to subscribe to all messages
		msgType = 0;
	}

	--pendingNotificationOps;
	notificationLock.Unlock();

	std::vector<cRZAutoRefCount<cIGZMessageTarget2> >::iterator it;
	for (it = targetsToNotify.begin(); it != targetsToNotify.end(); ++it)
	{
		(*it)->DoMessage(msg);
	}

	return true;
}

bool cGZMessageServer2::MessagePost(cIGZMessage2* msg, bool pushFront)
{
	cRZCriticalSectionHolder lock(messageQueueLock);
	pushFront ? messageQueue.PushFront(msg) : messageQueue.PushBack(msg);
	return true;
}

bool cGZMessageServer2::AddNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(notificationLock);
	if (pendingNotificationOps != 0)
	{
		delayedNotificationChanges.push_back(DelayedAddOrRemove(true, target, msgType));
		target->AddRef();
		return true;
	}

	pendingNotificationOps = 1;

	MessageTargetMap::iterator targetMapIt = notificationTargets.find(msgType);
	if (targetMapIt == notificationTargets.end())
	{
		MessageTargetList* list = new MessageTargetList();
		notificationTargets.insert(MessageTargetMapEntry(msgType, list));

		list->push_back(target);
	}
	else
	{
		MessageTargetList* list = targetMapIt->second;
		MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

		if (targetListIt == list->end())
		{
			list->push_back(target);
		}
	}

	--pendingNotificationOps;
	DoDelayedNotificationAdditionsAndRemovals();

	return true;
}

bool cGZMessageServer2::RemoveNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(notificationLock);
	if (pendingNotificationOps != 0)
	{
		delayedNotificationChanges.push_back(DelayedAddOrRemove(false, target, msgType));
		return true;
	}

	pendingNotificationOps = 1;

	MessageTargetMap::iterator targetMapIt = notificationTargets.find(msgType);
	if (targetMapIt != notificationTargets.end())
	{
		MessageTargetList* list = targetMapIt->second;
		MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

		if (targetListIt != list->end())
		{
			list->erase(targetListIt);
			if (list->empty())
			{
				delete list;
				notificationTargets.erase(msgType);
			}
		}
	}

	--pendingNotificationOps;
	DoDelayedNotificationAdditionsAndRemovals();

	return true;
}

void cGZMessageServer2::GeneralMessagePostToTarget(cIGZMessage2* msg, cIGZMessageTarget2* target)
{
	cRZCriticalSectionHolder lock(generalMessagePostLock);
	generalMessages.push_back(GeneralMessageTargetInfo(target, msg));
}

void cGZMessageServer2::CancelGeneralMessagePostsToTarget(cIGZMessageTarget2* target)
{
	cRZCriticalSectionHolder lock(generalMessagePostLock);
	GeneralMessageDeque::iterator it = generalMessages.begin();

	while (true)
	{
		if (generalMessages.empty())
		{
			break;
		}

		if (it->target == target)
		{
			it = generalMessages.erase(it);
		}
		else
		{
			++it;
		}
	}
}

bool cGZMessageServer2::OnTick(uint32_t tickCount)
{
	messageQueueLock.Lock();
	int32_t pendingMessageCount = messageQueue.GetMessageCount();
	uint64_t targetMessageCount = pendingMessageCount + totalMessagesDequeued;
	messageQueueLock.Unlock();

	do
	{
		do
		{
			messageQueueLock.Lock();

			cIGZMessage2* msg;
			bool haveMessage = messageQueue.GetFrontMessage(&msg);
			if (haveMessage)
			{
				++totalMessagesDequeued;
			}

			messageQueueLock.Unlock();
			if (!haveMessage)
			{
				goto processGeneralMessages;
			}

			MessageSend(msg);
		}
		while (alwaysClearQueueOnTick);
	}
	while (totalMessagesDequeued < targetMessageCount);

processGeneralMessages:
	generalMessagePostLock.Lock();
	if (generalMessages.empty())
	{
		generalMessagePostLock.Unlock();
	}
	else
	{
		generalMessagePostLock.Unlock();
		SendGeneralMessages();
	}

	return true;
}

int32_t cGZMessageServer2::GetMessageQueueSize()
{
	cRZCriticalSectionHolder lock(messageQueueLock);
	return messageQueue.GetMessageCount();
}

void cGZMessageServer2::SetAlwaysClearQueueOnTick(bool clearOnTick)
{
	this->alwaysClearQueueOnTick = clearOnTick;
}

uint32_t cGZMessageServer2::GetRefCount() const
{
	return cRZUnknown::RefCount();
}

bool cGZMessageServer2::CreateMessage(GZCLSID clsid, GZIID iid, void** msgOut)
{
	if (clsid == GZCLSID_cRZMessage2Standard)
	{
		cRZAutoRefCount<cIGZMessage2> msg = new cRZMessage2Standard();
		return msg->QueryInterface(iid, msgOut);
	}
	else
	{
		return false;
	}
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovals()
{
	static bool bAlreadyHere = false;

	cRZCriticalSectionHolder lock(notificationLock);
	if (!bAlreadyHere)
	{
		bAlreadyHere = true;
		if (pendingNotificationOps == 0)
		{
			DelayedNotificationList::iterator it = delayedNotificationChanges.begin();
			DelayedNotificationList::iterator itEnd = delayedNotificationChanges.end();

			while (it != itEnd)
			{
				if (!it->isAddition)
				{
					RemoveNotification(it->target, it->msgType);
				}
				else
				{
					AddNotification(it->target, it->msgType);
					it->target->Release();
				}

				it = delayedNotificationChanges.erase(it);
			}
		}

		bAlreadyHere = false;
	}
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
	for (DelayedNotificationList::iterator it = delayedNotificationChanges.begin(); it != delayedNotificationChanges.end(); ++it)
	{
		bool isAddition = it->isAddition;
		cIGZMessageTarget2* target = it->target;
		GZGUID msgType = it->msgType;

		MessageTargetMap::iterator targetMapIt = notificationTargets.find(it->msgType);

		// FUTURE: These inner blocks are just copied from the AddNotification and RemoveNotification implementations.
		// It would be nice to refactor these and make them inline methods.
		if (!isAddition)
		{
			if (targetMapIt != notificationTargets.end())
			{
				MessageTargetList* list = targetMapIt->second;
				MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

				if (targetListIt != list->end())
				{
					list->remove(target);
					if (list->empty())
					{
						delete list;
						notificationTargets.erase(msgType);
					}
				}
			}
		}
		else
		{
			if (targetMapIt == notificationTargets.end())
			{
				MessageTargetList* list = new MessageTargetList();
				notificationTargets.insert(MessageTargetMapEntry(msgType, list));
				list->push_back(target);
			}
			else
			{
				MessageTargetList* list = targetMapIt->second;
				MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

				if (targetListIt == list->end())
				{
					list->push_back(target);
				}
			}
		}
	}

	delayedNotificationChanges.clear();
}

void cGZMessageServer2::SendGeneralMessages()
{
	generalMessagePostLock.Lock();
	if (pendingGeneralMessageCount < 1)
	{
		int32_t size = generalMessages.size();
		pendingGeneralMessageCount = size;

		if (size < 1)
		{
			goto end;
		}
	}

	do
	{
		--pendingGeneralMessageCount;
		if (!generalMessages.empty())
		{
			GeneralMessageTargetInfo info(generalMessages.front());
			generalMessages.pop_front();

			generalMessagePostLock.Unlock();
			info.target->DoMessage(info.msg);
			generalMessagePostLock.Lock();
		}
	}
	while (pendingGeneralMessageCount > 0);

end:
	--pendingGeneralMessageCount; // FUTURE: This accounting just feels wrong
	generalMessagePostLock.Unlock();
}