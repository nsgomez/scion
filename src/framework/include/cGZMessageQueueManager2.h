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

#pragma once
#include <map>
#include "cIGZMessageQueue2.h"
#include "cIGZMessageQueueManager2.h"
#include "cRZAutoRefCount.h"
#include "cRZCriticalSection.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZMessageQueueManager2 = 0x64FA8434;

class cGZMessageQueueManager2 : public cIGZMessageQueueManager2, public cRZSystemService
{
public:
	cGZMessageQueueManager2();
	virtual ~cGZMessageQueueManager2();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Init();
	virtual bool Shutdown();

public:
	virtual bool GetQueue(GZGUID id, cIGZMessageQueue2** queueOut);

	virtual bool AddQueue(GZGUID id, cIGZMessageQueue2* queue);
	virtual bool AddQueue(GZGUID id);

	virtual bool RemoveQueue(cIGZMessageQueue2* queue);
	virtual bool RemoveQueue(GZGUID id);
	virtual void RemoveAllQueues();

	virtual uint32_t GetQueueCount();

protected:
	cRZCriticalSection mCriticalSection;
	std::map<GZGUID, cRZAutoRefCount<cIGZMessageQueue2> > mQueueMap;
};