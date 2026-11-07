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

#include "cGZMessageQueue2.h"
#include "cIGZMessage2.h"

static const uint32_t kBlockingCallWaitTimeInMicroseconds = 1000000;

cGZMessageQueue2::cGZMessageQueue2() :
	mMessages(),
	mCriticalSection(),
	mThreadSignal(false),
	mRefCount(0)
{
}

cGZMessageQueue2::~cGZMessageQueue2()
{
}

bool cGZMessageQueue2::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessageQueue2)
	{
		*outPtr = static_cast<cIGZMessageQueue2*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZMessageQueue2::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZMessageQueue2::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}
	else
	{
		delete this;
		return 0;
	}
}

void cGZMessageQueue2::PushBack(cIGZMessage2* msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	bool wasEmpty = mMessages.empty();
	mMessages.push_back(cRZAutoRefCount<cIGZMessage2>(msg));

	if (wasEmpty)
	{
		mThreadSignal.Signal();
	}
}

void cGZMessageQueue2::PushFront(cIGZMessage2* msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	bool wasEmpty = mMessages.empty();
	mMessages.push_front(cRZAutoRefCount<cIGZMessage2>(msg));

	if (wasEmpty)
	{
		mThreadSignal.Signal();
	}
}

bool cGZMessageQueue2::RemoveFirstFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (Message2ListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if ((**it) == msg)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue2::RemoveLastFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.end();
	while (it != mMessages.begin())
	{
		--it;
		if ((**it) == msg)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

int32_t cGZMessageQueue2::RemoveAllFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.begin();
	int removedCount = 0;

	while (it != mMessages.end())
	{
		if ((**it) == msg)
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

bool cGZMessageQueue2::RemoveFirstFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (Message2ListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if ((*it)->GetType() == msgType)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue2::RemoveLastFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.end();
	while (it != mMessages.begin())
	{
		--it;
		if ((*it)->GetType() == msgType)
		{
			mMessages.erase(it);
			return true;
		}
	}

	return false;
}

int32_t cGZMessageQueue2::RemoveAllFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.begin();
	int removedCount = 0;

	while (it != mMessages.end())
	{
		if ((*it)->GetType() == msgType)
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

int32_t cGZMessageQueue2::RemoveAll()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	int removedCount = mMessages.size();

	mMessages.clear();

	return removedCount;
}

GZGUID cGZMessageQueue2::PeekFrontType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type;

	if (!mMessages.empty())
	{
		type = mMessages.front()->GetType();
	}
	else
	{
		type = 0;
	}

	return type;
}

GZGUID cGZMessageQueue2::PeekBackType()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	GZGUID type = 0;

	if (!mMessages.empty())
	{
		type = mMessages.back()->GetType();
	}

	return type;
}

bool cGZMessageQueue2::PeekAnywhereType(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	for (Message2ListIterator it = mMessages.begin(); it != mMessages.end(); ++it)
	{
		if ((*it)->GetType() == msgType)
		{
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue2::PeekFrontMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool found;

	if (!mMessages.empty())
	{
		cIGZMessage2* msg = mMessages.front();
		*msgOut = msg;
		msg->AddRef();
		found = true;
	}
	else
	{
		*msgOut = NULL;
		found = false;
	}

	return found;
}

bool cGZMessageQueue2::PeekBackMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool found;

	if (!mMessages.empty())
	{
		cIGZMessage2* msg = mMessages.back();
		*msgOut = msg;
		msg->AddRef();
		found = true;
	}
	else
	{
		*msgOut = NULL;
		found = false;
	}

	return found;
}

bool cGZMessageQueue2::PeekAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.begin();
	while (it != mMessages.end())
	{
		if ((**it) == msgIn)
		{
			*msgOut = *it;
			(*msgOut)->AddRef();
			return true;
		}

		++it;
	}

	*msgOut = NULL;
	return false;
}

bool cGZMessageQueue2::GetFrontMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool found;

	if (!mMessages.empty())
	{
		cIGZMessage2* msg = mMessages.front();
		*msgOut = msg;
		msg->AddRef();

		mMessages.pop_front();
		found = true;
	}
	else
	{
		*msgOut = NULL;
		found = false;
	}

	return found;
}

bool cGZMessageQueue2::GetBackMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	bool found;

	if (!mMessages.empty())
	{
		cIGZMessage2* msg = mMessages.back();
		*msgOut = msg;
		msg->AddRef();

		mMessages.pop_back();
		found = true;
	}
	else
	{
		*msgOut = NULL;
		found = false;
	}

	return found;
}

bool cGZMessageQueue2::GetFrontMessageBlock(cIGZMessage2** msgOut)
{
	while (!GetFrontMessage(msgOut))
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

bool cGZMessageQueue2::GetBackMessageBlock(cIGZMessage2** msgOut)
{
	while (!GetBackMessage(msgOut))
	{
		mThreadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

int32_t cGZMessageQueue2::GetMessageCount()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	return mMessages.size();
}

bool cGZMessageQueue2::GetAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(mCriticalSection);

	Message2ListIterator it = mMessages.begin();
	while (it != mMessages.end())
	{
		if ((**it) == msgIn)
		{
			*msgOut = *it;
			(*msgOut)->AddRef();

			mMessages.erase(it);
			return true;
		}

		++it;
	}

	*msgOut = NULL;
	return false;
}