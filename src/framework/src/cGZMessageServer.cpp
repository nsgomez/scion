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

#include "cGZMessage.h"
#include "cGZMessageServer.h"
#include "cIGZFramework.h"
#include "cIGZMessageTarget.h"
#include "RZStatics.h"

cGZMessageServer::GeneralMessageTargetInfo::GeneralMessageTargetInfo(GeneralMessageTargetInfo const& other)
{
	this->target = other.target;
	this->msg.messageType = other.msg.messageType;
	this->msg.data1 = other.msg.data1;
	this->msg.data2 = other.msg.data2;
	this->msg.data3 = other.msg.data3;
}

cGZMessageServer::cGZMessageServer() :
	cRZSystemService(RZSRVID_cGZMessageServer, 5000000),
	alwaysClearQueueOnTick(true)
{
}

cGZMessageServer::~cGZMessageServer()
{
	cRZCriticalSectionHolder lock(notificationLock);
	pendingNotificationOps = 0;

	while (!notificationTargets.empty())
	{
		MessageTargetMap::iterator targetMapIt = notificationTargets.begin();
		MessageTargetList* list = targetMapIt->second;

		notificationTargets.erase(targetMapIt);
		if (list != NULL)
		{
			for (MessageTargetList::iterator listIt = list->begin(); listIt != list->end(); listIt = list->begin())
			{
				cIGZMessageTarget* target = *listIt;
				list->erase(listIt);
				target->Release();
			}

			delete list;
		}
	}
}

bool cGZMessageServer::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessageServer)
	{
		*outPtr = static_cast<cIGZMessageServer*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZMessageServer::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZMessageServer::Release()
{
	return cRZSystemService::Release();
}

bool cGZMessageServer::Init()
{
	cIGZFramework* framework = RZGetFramework();
	if (framework == NULL)
	{
		return false;
	}
	else
	{
		framework->AddToTick(this);
		return true;
	}
}

bool cGZMessageServer::Shutdown()
{
	cIGZFramework* framework = RZGetFramework();
	if (framework != NULL)
	{
		return framework->RemoveFromTick(this);
	}

	return true;
}

bool cGZMessageServer::MessageSend(cGZMessage const& msg)
{
	const int kMaxTargetsPerMessage = 384;
	cIGZMessageTarget* targetsToNotify[kMaxTargetsPerMessage];
	cIGZMessageTarget** currentTargetToNotify = targetsToNotify;

	notificationLock.Lock();
	DoDelayedNotificationAdditionsAndRemovalsImmediately();

	++pendingNotificationOps;
	GZGUID msgType = msg.messageType;

	if (msgType != 0)
	{
		MessageTargetMap::iterator targetMapIt = notificationTargets.find(msgType);
		if (targetMapIt != notificationTargets.end())
		{
			const MessageTargetList* targetList = targetMapIt->second;
			for (MessageTargetList::const_iterator it = targetList->begin(); it != targetList->end(); ++it)
			{
				cIGZMessageTarget* targetToAdd = *it;
				*(currentTargetToNotify++) = targetToAdd;
				targetToAdd->AddRef();
			}
		}
	}

	*currentTargetToNotify = NULL;
	--pendingNotificationOps;
	notificationLock.Unlock();

	currentTargetToNotify = targetsToNotify;
	while (targetsToNotify[0] != NULL)
	{
		targetsToNotify[0]->DoMessage(msg);
		targetsToNotify[0]->Release();

		++currentTargetToNotify;
		targetsToNotify[0] = *currentTargetToNotify;
	}

	return true;
}

bool cGZMessageServer::MessagePost(cGZMessage const& msg, bool pushFront)
{
	cRZCriticalSectionHolder lock(messageQueueLock);
	pushFront ? messageQueue.PushFront(msg) : messageQueue.PushBack(msg);
	return true;
}

bool cGZMessageServer::AddNotification(cIGZMessageTarget* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(notificationLock);
	if (pendingNotificationOps != 0)
	{
		delayedNotificationChanges.push_back(DelayedNotificationInfo(true, target, msgType));

		// FUTURE: There's a bug here and in cGZMessageServer2 where these deferred objects are not released
		// before the list is destroyed.
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
		target->AddRef();
	}
	else
	{
		MessageTargetList* list = targetMapIt->second;
		MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

		if (targetListIt == list->end())
		{
			list->push_back(target);
			target->AddRef();
		}
	}

	--pendingNotificationOps;
	DoDelayedNotificationAdditionsAndRemovals();

	return true;
}

bool cGZMessageServer::RemoveNotification(cIGZMessageTarget* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(notificationLock);
	if (pendingNotificationOps != 0)
	{
		delayedNotificationChanges.push_back(DelayedNotificationInfo(false, target, msgType));
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
			list->remove(target);
			target->Release();

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

void cGZMessageServer::GeneralMessagePostToTarget(cGZMessage const& msg, cIGZMessageTarget* target)
{
	cRZCriticalSectionHolder lock(generalMessagePostLock);
	generalMessages.push_back(GeneralMessageTargetInfo(target, msg));
}

void cGZMessageServer::CancelGeneralMessagePostsToTarget(cIGZMessageTarget* target)
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

bool cGZMessageServer::OnTick(uint32_t tickCount)
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

			cGZMessage msg;
			bool haveMessage = messageQueue.GetFrontMessage(msg);
			if (haveMessage)
			{
				++totalMessagesDequeued;
			}

			messageQueueLock.Unlock();
			if (!haveMessage)
			{
				goto processGeneralMessages;
			}

			static_cast<cIGZMessageServer*>(this)->MessageSend(msg);
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

int32_t cGZMessageServer::GetMessageQueueSize()
{
	cRZCriticalSectionHolder lock(messageQueueLock);
	return messageQueue.GetMessageCount();
}

void cGZMessageServer::SetAlwaysClearQueueOnTick(bool clearOnTick)
{
	this->alwaysClearQueueOnTick = clearOnTick;
}

void cGZMessageServer::DoDelayedNotificationAdditionsAndRemovals()
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

void cGZMessageServer::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
	for (DelayedNotificationList::iterator it = delayedNotificationChanges.begin(); it != delayedNotificationChanges.end(); ++it)
	{
		bool isAddition = it->isAddition;
		cIGZMessageTarget* target = it->target;
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
					target->Release();

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
				target->AddRef();
			}
			else
			{
				MessageTargetList* list = targetMapIt->second;
				MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

				if (targetListIt == list->end())
				{
					list->push_back(target);
					target->AddRef();
				}
			}
		}
	}

	delayedNotificationChanges.clear();
}

void cGZMessageServer::SendGeneralMessages()
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

			cIGZMessageTarget* target = info.target;
			target->DoMessage(info.msg);

			generalMessagePostLock.Lock();
		}
	}
	while (pendingGeneralMessageCount > 0);

end:
	--pendingGeneralMessageCount; // FUTURE: This accounting just feels wrong
	generalMessagePostLock.Unlock();
}