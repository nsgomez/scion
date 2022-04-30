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

#include "cGZMessageQueue.h"

static const uint32_t kBlockingCallWaitTimeInMicroseconds = 1000000;

cGZMessageQueue::cGZMessageQueue() :
	messages(),
	criticalSection(),
	threadSignal(false),
	refCount(0)
{
}

cGZMessageQueue::~cGZMessageQueue()
{
}

bool cGZMessageQueue::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessageQueue)
	{
		*outPtr = static_cast<cIGZMessageQueue*>(this);
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

uint32_t cGZMessageQueue::AddRef()
{
	return ++refCount;
}

uint32_t cGZMessageQueue::Release()
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

void cGZMessageQueue::PushBack(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	MessageListIterator it = messages.begin();
	messages.push_back(msg);

	if (it == messages.end())
	{
		threadSignal.Signal();
	}
}

void cGZMessageQueue::PushFront(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	MessageListIterator it = messages.begin();
	messages.push_front(msg);

	if (it == messages.end())
	{
		threadSignal.Signal();
	}
}

bool cGZMessageQueue::RemoveFirstFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (MessageListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if (it->messageType == msg.messageType
			&& it->data1 == msg.data1
			&& it->data2 == msg.data2
			&& it->data3 == msg.data3)
		{
			messages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue::RemoveLastFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	// Intentionally unimplemented
	return false;
}

int32_t cGZMessageQueue::RemoveAllFound(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);
	MessageListIterator it = messages.begin();
	MessageListIterator end = messages.end();
	int removedCount = 0;

	while (it != end)
	{
		if (it->messageType != msg.messageType
			|| it->data1 != msg.data1
			|| it->data2 != msg.data2
			|| it->data3 != msg.data3)
		{
			++it;
			continue;
		}

		it = messages.erase(it);
		++removedCount;
	}

	return removedCount;
}

bool cGZMessageQueue::RemoveFirstFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (MessageListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if (it->messageType == msgType)
		{
			messages.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueue::RemoveLastFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	// Intentionally unimplemented
	return false;
}

int32_t cGZMessageQueue::RemoveAllFound(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);
	MessageListIterator it = messages.begin();
	MessageListIterator end = messages.end();
	int removedCount = 0;

	while (it != end)
	{
		if (it->messageType != msgType)
		{
			++it;
			continue;
		}

		it = messages.erase(it);
		++removedCount;
	}

	return removedCount;
}

int32_t cGZMessageQueue::RemoveAll()
{
	cRZCriticalSectionHolder lock(criticalSection);
	int removedCount = messages.size();

	messages.clear();

	return removedCount;
}

GZGUID cGZMessageQueue::PeekFrontType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.front().messageType;
	}

	return type;
}

GZGUID cGZMessageQueue::PeekBackType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.back().messageType;
	}

	return type;
}

bool cGZMessageQueue::PeekAnywhereType(GZGUID msgType)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (MessageListIterator it = messages.begin(); it != messages.end(); ++it)
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
	cRZCriticalSectionHolder lock(criticalSection);
	cGZMessage* msg = NULL;

	if (!messages.empty())
	{
		msg = &messages.front();
	}

	return msg;
}

cGZMessage const* cGZMessageQueue::PeekBackMessage()
{
	cRZCriticalSectionHolder lock(criticalSection);
	cGZMessage* msg = NULL;

	if (!messages.empty())
	{
		msg = &messages.back();
	}

	return msg;
}

bool cGZMessageQueue::PeekAnywhereMessage(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (MessageListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if (it->messageType == msg.messageType
			&& it->data1 == msg.data1
			&& it->data2 == msg.data2
			&& it->data3 == msg.data3)
		{
			return true;
		}
	}

	return false;
}

GZGUID cGZMessageQueue::GetFrontType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.front().messageType;
		messages.pop_front();
	}

	return type;
}

GZGUID cGZMessageQueue::GetBackType()
{
	cRZCriticalSectionHolder lock(criticalSection);
	GZGUID type = 0;

	if (!messages.empty())
	{
		type = messages.back().messageType;
		messages.pop_back();
	}

	return type;
}

bool cGZMessageQueue::GetAnywhereType(GZGUID msgType)
{
	return RemoveFirstFound(msgType);
}

bool cGZMessageQueue::GetFrontMessage(cGZMessage& msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	if (!messages.empty())
	{
		msgOut = messages.front();
		messages.pop_front();
		return true;
	}

	return false;
}

bool cGZMessageQueue::GetBackMessage(cGZMessage& msgOut)
{
	cRZCriticalSectionHolder lock(criticalSection);

	if (!messages.empty())
	{
		msgOut = messages.back();
		messages.pop_back();
		return true;
	}

	return false;
}

bool cGZMessageQueue::GetAnywhereMessage(cGZMessage const& msg)
{
	cRZCriticalSectionHolder lock(criticalSection);

	for (MessageListIterator it = messages.begin(); it != messages.end(); ++it)
	{
		if (it->messageType == msg.messageType
			&& it->data1 == msg.data1
			&& it->data2 == msg.data2
			&& it->data3 == msg.data3)
		{
			messages.erase(it);
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
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
		type = GetFrontType();
	}

	return type;
}

GZGUID cGZMessageQueue::GetBackTypeBlock()
{
	GZGUID type = GetBackType();
	while (type == 0)
	{
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
		type = GetBackType();
	}

	return type;
}

bool cGZMessageQueue::GetFrontMessageBlock(cGZMessage& msgOut)
{
	while (!GetFrontMessage(msgOut))
	{
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

bool cGZMessageQueue::GetBackMessageBlock(cGZMessage& msgOut)
{
	while (!GetBackMessage(msgOut))
	{
		threadSignal.TimedWait(kBlockingCallWaitTimeInMicroseconds);
	}

	return true;
}

int32_t cGZMessageQueue::GetMessageCount()
{
	cRZCriticalSectionHolder lock(criticalSection);
	return messages.size();
}