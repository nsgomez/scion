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
	mTotalMessagesDequeued(0),
	mAlwaysClearQueueOnTick(true),
	mPendingNotificationOps(0),
	mPendingGeneralMessageCount(0)
{
}

cGZMessageServer2::~cGZMessageServer2()
{
	mNotificationLock.Lock();

	mPendingNotificationOps = 0;
	while (!mNotificationTargets.empty())
	{
		MessageTargetMap::iterator it = mNotificationTargets.begin();
		MessageTargetList* list = it->second;
		mNotificationTargets.erase(it);

		if (list != NULL)
		{
			while (!list->empty())
			{
				__try
				{
					cRZAutoRefCount<cIGZMessageTarget2> target = list->front();
					list->pop_front();
				}
				__finally { }
			}
		}

		delete list;
	}

	mNotificationLock.Unlock();
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

	mNotificationLock.Lock();
	DoDelayedNotificationAdditionsAndRemovalsImmediately();

	++mPendingNotificationOps;
	GZGUID msgType = msg->GetType();

	while (true)
	{
		MessageTargetMap::iterator targetMapIt = mNotificationTargets.find(msgType);
		if (targetMapIt != mNotificationTargets.end())
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

	--mPendingNotificationOps;
	mNotificationLock.Unlock();

	std::vector<cRZAutoRefCount<cIGZMessageTarget2> >::iterator it;
	for (it = targetsToNotify.begin(); it != targetsToNotify.end(); ++it)
	{
		(*it)->DoMessage(msg);
	}

	return true;
}

bool cGZMessageServer2::MessagePost(cIGZMessage2* msg, bool pushFront)
{
	cRZCriticalSectionHolder lock(mMessageQueueLock);
	pushFront ? mMessageQueue.PushFront(msg) : mMessageQueue.PushBack(msg);
	return true;
}

bool cGZMessageServer2::AddNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mNotificationLock);
	if (mPendingNotificationOps != 0)
	{
		mDelayedNotificationChanges.push_back(DelayedAddOrRemove(true, DelayedAddOrRemove::TargetAndType(target, msgType)));
		return true;
	}

	mPendingNotificationOps = 1;

	MessageTargetList* list;
	MessageTargetList::iterator targetListIt;
	MessageTargetMap::iterator targetMapIt = mNotificationTargets.find(msgType);

	if (targetMapIt == mNotificationTargets.end())
	{
		list = new MessageTargetList();
		mNotificationTargets.insert(MessageTargetMapEntry(msgType, list));
	}
	else
	{
		list = targetMapIt->second;
		targetListIt = list->begin();

		while (targetListIt != list->end())
		{
			if (target == *targetListIt)
			{
				goto found;
			}

			++targetListIt;
		}
	}

	list->push_back(NULL);
	list->back() = target;

found:
	--mPendingNotificationOps;
	DoDelayedNotificationAdditionsAndRemovals();

	return true;
}

bool cGZMessageServer2::RemoveNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mNotificationLock);
	if (mPendingNotificationOps != 0)
	{
		mDelayedNotificationChanges.push_back(DelayedAddOrRemove(false, DelayedAddOrRemove::TargetAndType(target, msgType)));
		return true;
	}

	mPendingNotificationOps = 1;

	MessageTargetMap::iterator targetMapIt = mNotificationTargets.find(msgType);
	if (targetMapIt != mNotificationTargets.end())
	{
		MessageTargetList* list = targetMapIt->second;
		MessageTargetList::iterator targetListIt = list->begin();

		while (targetListIt != list->end())
		{
			if (target == *targetListIt)
			{
				break;
			}

			++targetListIt;
		}

		if (targetListIt != list->end())
		{
			list->erase(targetListIt);
			if (list->empty())
			{
				delete list;
				mNotificationTargets.erase(msgType);
			}
		}
	}

	--mPendingNotificationOps;
	DoDelayedNotificationAdditionsAndRemovals();

	return true;
}

void cGZMessageServer2::GeneralMessagePostToTarget(cIGZMessage2* msg, cIGZMessageTarget2* target)
{
	cRZCriticalSectionHolder lock(mGeneralMessagePostLock);
	mGeneralMessages.push_back(GeneralMessageTargetInfo(target, msg));
}

void cGZMessageServer2::CancelGeneralMessagePostsToTarget(cIGZMessageTarget2* target)
{
	cRZCriticalSectionHolder lock(mGeneralMessagePostLock);
	GeneralMessageDeque::iterator it = mGeneralMessages.begin();

	while (it != mGeneralMessages.end())
	{
		if (it->target == target)
		{
			GeneralMessageDeque::iterator original = it;
			++it;
			mGeneralMessages.erase(original);
		}
		else
		{
			++it;
		}
	}
}

bool cGZMessageServer2::OnTick(uint32_t tickCount)
{
	cIGZMessage2* msg = NULL;

	mMessageQueueLock.Lock();
	uint32_t pendingMessageCount = mMessageQueue.GetMessageCount();
	uint64_t targetMessageCount = pendingMessageCount + mTotalMessagesDequeued;
	mMessageQueueLock.Unlock();

	do
	{
		do
		{
			mMessageQueueLock.Lock();

			cIGZMessage2* msg;
			bool haveMessage = mMessageQueue.GetFrontMessage(&msg);
			if (haveMessage)
			{
				++mTotalMessagesDequeued;
			}

			mMessageQueueLock.Unlock();
			if (!haveMessage)
			{
				goto processGeneralMessages;
			}

			MessageSend(msg);
			msg->Release();
		}
		while (mAlwaysClearQueueOnTick);
	}
	while (mTotalMessagesDequeued < targetMessageCount);

processGeneralMessages:
	mGeneralMessagePostLock.Lock();
	if (!mGeneralMessages.empty())
	{
		mGeneralMessagePostLock.Unlock();
		SendGeneralMessages();
	}
	else
	{
		mGeneralMessagePostLock.Unlock();
	}

	return true;
}

int32_t cGZMessageServer2::GetMessageQueueSize()
{
	cRZCriticalSectionHolder lock(mMessageQueueLock);
	return mMessageQueue.GetMessageCount();
}

void cGZMessageServer2::SetAlwaysClearQueueOnTick(bool clearOnTick)
{
	this->mAlwaysClearQueueOnTick = clearOnTick;
}

uint32_t cGZMessageServer2::GetRefCount() const
{
	return RefCount();
}

bool cGZMessageServer2::CreateMessage(GZCLSID clsid, GZIID iid, void** msgOut)
{
	if (clsid != GZCLSID_cRZMessage2Standard)
	{
		return false;
	}
	
	cRZAutoRefCount<cIGZMessage2> msg = new cRZMessage2Standard();
	return msg->QueryInterface(iid, msgOut);
}

cIGZSystemService* cGZMessageServer2::Create()
{
	return new cGZMessageServer2();
}

GZCLSID cGZMessageServer2::GetGZCLSID()
{
	return GZCLSID_cGZMessageServer2;
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovals()
{
	static bool bAlreadyHere = false;

	cRZCriticalSectionHolder lock(mNotificationLock);
	if (!bAlreadyHere)
	{
		bAlreadyHere = true;
		if (mPendingNotificationOps == 0)
		{
			DelayedNotificationList::iterator it = mDelayedNotificationChanges.begin();
			DelayedNotificationList::iterator itEnd = mDelayedNotificationChanges.end();

			while (it != itEnd)
			{
				if (!it->isAddition)
				{
					RemoveNotification(it->info.target, it->info.msgType);
				}
				else
				{
					AddNotification(it->info.target, it->info.msgType);
					it->info.target->Release();
				}

				it = mDelayedNotificationChanges.erase(it);
			}
		}

		bAlreadyHere = false;
	}
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
	for (DelayedNotificationList::iterator it = mDelayedNotificationChanges.begin(); it != mDelayedNotificationChanges.end(); ++it)
	{
		bool isAddition = it->isAddition;
		cIGZMessageTarget2* target = it->info.target;
		GZGUID msgType = it->info.msgType;

		MessageTargetMap::iterator targetMapIt = mNotificationTargets.find(it->info.msgType);

		// FUTURE: These inner blocks are just copied from the AddNotification and RemoveNotification implementations.
		// It would be nice to refactor these and make them inline methods.
		if (!isAddition)
		{
			if (targetMapIt != mNotificationTargets.end())
			{
				MessageTargetList* list = targetMapIt->second;
				MessageTargetList::iterator targetListIt = std::find(list->begin(), list->end(), target);

				if (targetListIt != list->end())
				{
					list->remove(target);
					if (list->empty())
					{
						delete list;
						mNotificationTargets.erase(msgType);
					}
				}
			}
		}
		else
		{
			if (targetMapIt == mNotificationTargets.end())
			{
				MessageTargetList* list = new MessageTargetList();
				mNotificationTargets.insert(MessageTargetMapEntry(msgType, list));
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

	mDelayedNotificationChanges.clear();
}

void cGZMessageServer2::SendGeneralMessages()
{
	mGeneralMessagePostLock.Lock();
	if (mPendingGeneralMessageCount < 1)
	{
		int32_t size = mGeneralMessages.size();
		mPendingGeneralMessageCount = size;

		if (size < 1)
		{
			goto end;
		}
	}

	do
	{
		--mPendingGeneralMessageCount;
		if (!mGeneralMessages.empty())
		{
			GeneralMessageTargetInfo info(mGeneralMessages.front());
			mGeneralMessages.pop_front();

			mGeneralMessagePostLock.Unlock();
			info.target->DoMessage(info.msg);
			mGeneralMessagePostLock.Lock();
		}
	}
	while (mPendingGeneralMessageCount > 0);

end:
	--mPendingGeneralMessageCount; // FUTURE: This accounting just feels wrong
	mGeneralMessagePostLock.Unlock();
}