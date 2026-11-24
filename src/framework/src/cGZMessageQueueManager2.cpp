/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cGZMessageQueueManager2.h"
#include "cRZCriticalSectionHolder.h"

cGZMessageQueueManager2::cGZMessageQueueManager2() :
	cRZSystemService(RZSRVID_cGZMessageQueueManager2, 5000000)
{
}

cGZMessageQueueManager2::~cGZMessageQueueManager2()
{
}

bool cGZMessageQueueManager2::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessageQueueManager2)
	{
		*outPtr = static_cast<cIGZMessageQueueManager2*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZMessageQueueManager2::AddRef()
{
	return cRZUnknown::AddRef();
}

uint32_t cGZMessageQueueManager2::Release()
{
	return cRZUnknown::Release();
}

bool cGZMessageQueueManager2::Init()
{
	return true;
}

bool cGZMessageQueueManager2::Shutdown()
{
	return true;
}

bool cGZMessageQueueManager2::GetQueue(GZGUID id, cIGZMessageQueue2** queueOut)
{
	cRZCriticalSectionHolder lock(*this);

	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator it = mQueueMap.find(id);
	if (it != mQueueMap.end())
	{
		*queueOut = it->second;
		(*queueOut)->AddRef();
	}
	else
	{
		*queueOut = NULL;
	}

	return *queueOut != NULL;
}

bool cGZMessageQueueManager2::AddQueue(GZGUID id)
{
	cRZCriticalSectionHolder lock(*this);

	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator it = mQueueMap.find(id);
	if (it == mQueueMap.end())
	{
		mQueueMap.insert(std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::value_type(id, new cGZMessageQueue2()));
		return true;
	}
	else
	{
		return false;
	}
}

bool cGZMessageQueueManager2::AddQueue(GZGUID id, cIGZMessageQueue2* queue)
{
	cRZCriticalSectionHolder lock(*this);

	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator it = mQueueMap.find(id);
	if (it == mQueueMap.end())
	{
		mQueueMap.insert(std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::value_type(id, queue));
		return true;
	}
	else
	{
		return false;
	}
}

bool cGZMessageQueueManager2::RemoveQueue(GZGUID id)
{
	cRZCriticalSectionHolder lock(*this);

	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator it = mQueueMap.find(id);
	if (it != mQueueMap.end())
	{
		mQueueMap.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

bool cGZMessageQueueManager2::RemoveQueue(cIGZMessageQueue2* queue)
{
	cRZCriticalSectionHolder lock(*this);

	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator end = mQueueMap.end();
	for (std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> >::iterator it = mQueueMap.begin(); it != end; it++)
	{
		if (it->second == queue)
		{
			mQueueMap.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZMessageQueueManager2::RemoveAllQueues()
{
	cRZCriticalSectionHolder lock(*this);
	mQueueMap.clear();
	return true;
}

uint32_t cGZMessageQueueManager2::GetQueueCount()
{
	return mQueueMap.size();
}