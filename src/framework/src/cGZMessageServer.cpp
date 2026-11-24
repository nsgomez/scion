/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGZMessage.h"
#include "cGZMessageServer.h"
#include "cIGZFramework.h"
#include "cIGZMessageTarget.h"
#include "cRZCriticalSectionHolder.h"
#include "RZStatics.h"

cGZMessageServer::GeneralMessageTargetInfo::GeneralMessageTargetInfo(GeneralMessageTargetInfo const& other)
{
	this->target = other.target;
	this->msg.messageType = other.msg.messageType;
	this->msg.data1 = other.msg.data1;
	this->msg.data2 = other.msg.data2;
	this->msg.data3 = other.msg.data3;
}

cGZMessageServer::GeneralMessageTargetInfo::GeneralMessageTargetInfo(cIGZMessageTarget* target, cGZMessage const& msg) :
	target(target),
	msg(msg)
{
}

cGZMessageServer::cGZMessageServer() :
	cRZSystemService(RZSRVID_cGZMessageServer, 5000000),
	totalMessagesDequeued(0),
	pendingNotificationOps(0),
	alwaysClearQueueOnTick(true),
	pendingGeneralMessageCount(0)
{
}

cGZMessageServer::~cGZMessageServer()
{
	notificationLock.Lock();
	pendingNotificationOps = 0;

	while (!notificationTargets.empty())
	{
		MessageTargetMap::iterator targetMapIt = notificationTargets.begin();
		MessageTargetList* list = targetMapIt->second;

		notificationTargets.erase(targetMapIt);
		if (list != NULL)
		{
			while (!list->empty())
			{
				cIGZMessageTarget* target = list->front();
				list->pop_front();

				__try
				{
					target->Release();
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
				}
			}

			delete list;
		}
	}

	notificationLock.Unlock();
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
	return cRZUnknown::AddRef();
}

uint32_t cGZMessageServer::Release()
{
	return cRZUnknown::Release();
}

bool cGZMessageServer::Init()
{
	cIGZFramework* framework = RZGetFramework();
	if (framework != NULL)
	{
		return framework->AddToTick(this);
	}

	return false;
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
	cIGZMessageTarget* target = *currentTargetToNotify;

	while (target != NULL)
	{
		target->DoMessage(msg);
		target->Release();

		++currentTargetToNotify;
		target = *currentTargetToNotify;
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
		MessageTargetList::iterator end = list->end();

		if (std::find(list->begin(), end, target) == end)
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
		MessageTargetList::iterator end = list->end();

		if (std::find(list->begin(), end, target) != end)
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

	while (it != generalMessages.end())
	{
		if (it->target == target)
		{
			generalMessages.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

bool cGZMessageServer::OnTick(uint32_t tickCount)
{
	cGZMessage msg(0);

	messageQueueLock.Lock();
	uint32_t pendingMessageCount = messageQueue.GetMessageCount();
	uint64_t targetMessageCount = pendingMessageCount + totalMessagesDequeued;
	messageQueueLock.Unlock();

	do
	{
		do
		{
			messageQueueLock.Lock();

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
	if (!generalMessages.empty())
	{
		generalMessagePostLock.Unlock();
		SendGeneralMessages();
	}
	else
	{
		generalMessagePostLock.Unlock();
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
	cRZCriticalSectionHolder lock(notificationLock);
	static bool bAlreadyHere = false;

	if (!bAlreadyHere)
	{
		bAlreadyHere = true;
		if (pendingNotificationOps == 0)
		{
			if (!delayedNotificationChanges.empty())
			{
				cIGZMessageServer* server = static_cast<cIGZMessageServer*>(this);

				do
				{
					DelayedNotificationInfo& change = delayedNotificationChanges.front();
					if (change.isAddition)
					{
						server->AddNotification(change.target, change.msgType);
						change.target->Release();
					}
					else
					{
						server->RemoveNotification(change.target, change.msgType);
					}
				}
				while (!delayedNotificationChanges.empty());
			}
		}

		bAlreadyHere = false;
	}
}

void cGZMessageServer::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
	if (!delayedNotificationChanges.empty())
	{
		for (DelayedNotificationList::iterator it = delayedNotificationChanges.begin(); it != delayedNotificationChanges.end(); ++it)
		{
			if (it->isAddition)
			{
				MessageTargetMap::iterator targetMapIt = notificationTargets.find(it->msgType);
				if (targetMapIt == notificationTargets.end())
				{
					MessageTargetList* list = new MessageTargetList();
					notificationTargets.insert(MessageTargetMapEntry(it->msgType, list));
					list->push_back(it->target);
				}
				else
				{
					MessageTargetList* list = targetMapIt->second;
					if (std::find(list->begin(), list->end(), it->target) == list->end())
					{
						list->push_back(it->target);
					}
				}
			}
			else
			{
				MessageTargetMap::iterator targetMapIt = notificationTargets.find(it->msgType);
				if (targetMapIt != notificationTargets.end())
				{
					MessageTargetList* list = targetMapIt->second;

					if (std::find(list->begin(), list->end(), it->target) != list->end())
					{
						list->remove(it->target);
						it->target->Release();

						if (list->empty())
						{
							delete list;
							notificationTargets.erase(it->msgType);
						}
					}
				}
			}
		}
	}

	delayedNotificationChanges.clear();
}

void cGZMessageServer::SendGeneralMessages()
{
	GeneralMessageTargetInfo info;

	generalMessagePostLock.Lock();
	if (pendingGeneralMessageCount <= 0)
	{
		pendingGeneralMessageCount = generalMessages.size();
		if (pendingGeneralMessageCount <= 0)
		{
			goto end;
		}
	}

	do
	{
		--pendingGeneralMessageCount;
		if (!generalMessages.empty())
		{
			info = generalMessages.front();
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