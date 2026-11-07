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

#include "cGZMessageQueue.h"

static const uint32_t kBlockingCallWaitTimeInMicroseconds = 1000000;

cGZMessageQueue::cGZMessageQueue() :
	mMessages(),
	mCriticalSection(),
	mThreadSignal(false),
	mRefCount(0)
{
}

cGZMessageQueue::~cGZMessageQueue()
{
}

bool cGZMessageQueue::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	case GZIID_cIGZMessageQueue:
		*outPtr = static_cast<cIGZMessageQueue*>(this);
		AddRef();
		return true;

	default:
		return false;
	}
}

uint32_t cGZMessageQueue::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZMessageQueue::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

void cGZMessageQueue::PushBack(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	const bool wasEmpty = mMessages.empty();
	mMessages.push_back(msg);

	if (wasEmpty)
	{
		mThreadSignal.Signal();
	}
}

void cGZMessageQueue::PushFront(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	const bool wasEmpty = mMessages.empty();
	mMessages.push_front(msg);

	if (wasEmpty)
	{
		mThreadSignal.Signal();
	}
}

bool cGZMessageQueue::RemoveFirstFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (MessageListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if (*it == msg)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue::RemoveLastFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	// Intentionally unimplemented
	return false;
}

int32_t cGZMessageQueue::RemoveAllFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	MessageListIterator it = mMessages.begin();
	int removedCount = 0;

	while (it != mMessages.end())
	{
		if (*it == msg)
		{
			++removedCount;
			mMessages.erase(it++);
		}
		else
		{
			++it;
		}
	}

	return removedCount;
}

bool cGZMessageQueue::RemoveFirstFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (MessageListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if (it->messageType == msgType)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue::RemoveLastFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	// Intentionally unimplemented
	return false;
}

int32_t cGZMessageQueue::RemoveAllFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	MessageListIterator it = mMessages.begin();
	int removedCount = 0;

	while (it != mMessages.end())
	{
		if (it->messageType == msgType)
		{
			++removedCount;
			mMessages.erase(it++);
		}
		else
		{
			++it;
		}
	}

	return removedCount;
}

int32_t cGZMessageQueue::RemoveAll()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	int removedCount = mMessages.size();

	mMessages.clear();

	return removedCount;
}

GZGUID cGZMessageQueue::PeekFrontType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type;

	if (!mMessages.empty())
	{
		type = mMessages.front().messageType;
	}
	else
	{
		type = 0;
	}

	return type;
}

GZGUID cGZMessageQueue::PeekBackType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type;

	if (!mMessages.empty())
	{
		type = mMessages.back().messageType;
	}
	else
	{
		type = 0;
	}

	return type;
}

bool cGZMessageQueue::PeekAnywhereType(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (MessageListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if (it->messageType == msgType)
		{
			return true;
		}
	}

	return false;
}

cGZMessage const* cGZMessageQueue::PeekFrontMessage()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	if (!mMessages.empty())
	{
		return &mMessages.front();
	}
	else
	{
		return NULL;
	}
}

cGZMessage const* cGZMessageQueue::PeekBackMessage()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	if (!mMessages.empty())
	{
		return &mMessages.back();
	}
	else
	{
		return NULL;
	}
}

bool cGZMessageQueue::PeekAnywhereMessage(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	for (MessageListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if (*it == msg)
		{
			return true;
		}
	}

	return false;
}

GZGUID cGZMessageQueue::GetFrontType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type;

	if (!mMessages.empty())
	{
		type = mMessages.front().messageType;
		mMessages.pop_front();
	}
	else
	{
		type = 0;
	}

	return type;
}

GZGUID cGZMessageQueue::GetBackType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type;

	if (!mMessages.empty())
	{
		type = mMessages.back().messageType;
		mMessages.pop_back();
	}
	else
	{
		type = 0;
	}

	return type;
}

bool cGZMessageQueue::GetAnywhereType(GZGUID msgType)
{
	return RemoveFirstFound(msgType);
}

bool cGZMessageQueue::GetFrontMessage(cGZMessage& msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool result;

	if (!mMessages.empty())
	{
		msgOut = mMessages.front();
		mMessages.pop_front();
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}

bool cGZMessageQueue::GetBackMessage(cGZMessage& msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool result;

	if (!mMessages.empty())
	{
		msgOut = mMessages.back();
		mMessages.pop_back();
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}

bool cGZMessageQueue::GetAnywhereMessage(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (MessageListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if (*it == msg)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

GZGUID cGZMessageQueue::GetFrontTypeBlock()
{
	GZGUID type = GetFrontType();
	while (type == 0)
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
		type = GetFrontType();
	}

	return type;
}

GZGUID cGZMessageQueue::GetBackTypeBlock()
{
	GZGUID type = GetBackType();
	while (type == 0)
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
		type = GetBackType();
	}

	return type;
}

bool cGZMessageQueue::GetFrontMessageBlock(cGZMessage& msgOut)
{
	while (!GetFrontMessage(msgOut))
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

bool cGZMessageQueue::GetBackMessageBlock(cGZMessage& msgOut)
{
	while (!GetBackMessage(msgOut))
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

int32_t cGZMessageQueue::GetMessageCount()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	return mMessages.size();
}