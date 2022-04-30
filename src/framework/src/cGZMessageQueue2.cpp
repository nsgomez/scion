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

#include "cGZMessageQueue2.h"
#include "cIGZMessage2.h"

static const uint32_t kBlockingCallWaitTimeInMicroseconds = 1000000;

cGZMessageQueue2::cGZMessageQueue2() :
	messages(),
	criticalSection(),
	threadSignal(false),
	refCount(0)
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
	return ++refCount;
}

uint32_t cGZMessageQueue2::Release()
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

void cGZMessageQueue2::PushBack(cIGZMessage2* msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	Message2ListIterator it = messages.begin();
	messages.push_back(msg);

	if (it == messages.end())
	{
		threadSignal.Signal();
	}
}

void cGZMessageQueue2::PushFront(cIGZMessage2* msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	Message2ListIterator it = messages.begin();
	messages.push_front(msg);

	if (it == messages.end())
	{
		threadSignal.Signal();
	}
}

bool cGZMessageQueue2::RemoveFirstFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if ((**it) == msg)
		{
			messages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue2::RemoveLastFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListReverseIterator it = messages.rbegin(); it != messages.rend(); ++it)
	{
		if ((**it) == msg)
		{
			messages.erase(it.base());
			return true;
		}
	}

	return false;
}

int32_t cGZMessageQueue2::RemoveAllFound(cIGZMessage2 const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);
	int removedCount = 0;
	Message2ListIterator it = messages.begin();

	while (it != messages.end())
	{
		Message2ListIterator tmp = it;
		bool isMatch = (**tmp) == msg;
		++it;

		if (isMatch)
		{
			messages.erase(tmp);
			++removedCount;
		}
	}

	return removedCount;
}

bool cGZMessageQueue2::RemoveFirstFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if ((*it)->GetType() == msgType)
		{
			messages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue2::RemoveLastFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListReverseIterator it = messages.rbegin(); it != messages.rend(); ++it)
	{
		if ((*it)->GetType() == msgType)
		{
			messages.erase(it.base());
			return true;
		}
	}

	return false;
}

int32_t cGZMessageQueue2::RemoveAllFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);
	int removedCount = 0;
	Message2ListIterator it = messages.begin();

	while (it != messages.end())
	{
		Message2ListIterator tmp = it;
		bool isMatch = (*it)->GetType() == msgType;
		++it;

		if (isMatch)
		{
			messages.erase(tmp);
			++removedCount;
		}
	}

	return removedCount;
}

int32_t cGZMessageQueue2::RemoveAll()
{
	cRZCriticalSectionHolder lock(criticalSection);
	int removedCount = messages.size();

	messages.clear();

	return removedCount;
}

GZGUID cGZMessageQueue2::PeekFrontType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.front()->GetType();
	}

	return type;
}

GZGUID cGZMessageQueue2::PeekBackType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.back()->GetType();
	}

	return type;
}

bool cGZMessageQueue2::PeekAnywhereType(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListIterator it = messages.begin(); it != messages.end(); ++it)
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
	cRZCriticalSectionHolder lock(criticalSection);

	if (messages.empty())
	{
		*msgOut = NULL;
	}
	else
	{
		cIGZMessage2* msg = messages.front();
		*msgOut = msg;
		msg->AddRef();
	}

	return !messages.empty();
}

bool cGZMessageQueue2::PeekBackMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	if (messages.empty())
	{
		*msgOut = NULL;
	}
	else
	{
		cIGZMessage2* msg = messages.back();
		*msgOut = msg;
		msg->AddRef();
	}

	return !messages.empty();
}

bool cGZMessageQueue2::PeekAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		cIGZMessage2* msg = *it;
		if ((*msg) == msgIn)
		{
			*msgOut = msg;
			msg->AddRef();
			return true;
		}
	}

	*msgOut = NULL;
	return false;
}

bool cGZMessageQueue2::GetFrontMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	if (messages.empty())
	{
		*msgOut = NULL;
	}
	else
	{
		cIGZMessage2* msg = messages.front();
		*msgOut = msg;
		msg->AddRef();

		messages.pop_front();
		return true;
	}

	return false;
}

bool cGZMessageQueue2::GetBackMessage(cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	if (messages.empty())
	{
		*msgOut = NULL;
	}
	else
	{
		cIGZMessage2* msg = messages.back();
		*msgOut = msg;
		msg->AddRef();

		messages.pop_back();
		return true;
	}

	return false;
}

bool cGZMessageQueue2::GetFrontMessageBlock(cIGZMessage2** msgOut)
{
	while (!GetFrontMessage(msgOut))
	{
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

bool cGZMessageQueue2::GetBackMessageBlock(cIGZMessage2** msgOut)
{
	while (!GetBackMessage(msgOut))
	{
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

int32_t cGZMessageQueue2::GetMessageCount()
{
	cRZCriticalSectionHolder lock(criticalSection);
	return messages.size();
}

bool cGZMessageQueue2::GetAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (Message2ListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		cIGZMessage2* msg = *it;
		if ((*msg) == msgIn)
		{
			*msgOut = msg;
			msg->AddRef();

			messages.erase(it);
			return true;
		}
	}

	*msgOut = NULL;
	return false;
}