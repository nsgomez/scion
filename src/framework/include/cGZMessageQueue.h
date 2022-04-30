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

#pragma once
#include <list>
#include "cGZMessage.h"
#include "cIGZMessageQueue.h"
#include "cRZCriticalSection.h"
#include "cRZThreadSignal.h"

class cGZMessageQueue : public cIGZMessageQueue
{
public:
	cGZMessageQueue();
	virtual ~cGZMessageQueue();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual void PushBack(cGZMessage const& msg);
	virtual void PushFront(cGZMessage const& msg);

	virtual bool RemoveFirstFound(cGZMessage const& msg);
	virtual bool RemoveLastFound(cGZMessage const& msg);
	virtual int32_t RemoveAllFound(cGZMessage const& msg);

	virtual bool RemoveFirstFound(GZGUID msgType);
	virtual bool RemoveLastFound(GZGUID msgType);
	virtual int32_t RemoveAllFound(GZGUID msgType);

	virtual int32_t RemoveAll();

	virtual GZGUID PeekFrontType();
	virtual GZGUID PeekBackType();
	virtual bool PeekAnywhereType(GZGUID msgType);

	virtual cGZMessage const* PeekFrontMessage();
	virtual cGZMessage const* PeekBackMessage();
	virtual bool PeekAnywhereMessage(cGZMessage const& msg);

	virtual GZGUID GetFrontType();
	virtual GZGUID GetBackType();
	virtual bool GetAnywhereType(GZGUID msgType);

	virtual bool GetFrontMessage(cGZMessage& msgOut);
	virtual bool GetBackMessage(cGZMessage& msgOut);
	virtual bool GetAnywhereMessage(cGZMessage const& msg);

	virtual GZGUID GetFrontTypeBlock();
	virtual GZGUID GetBackTypeBlock();

	virtual bool GetFrontMessageBlock(cGZMessage& msgOut);
	virtual bool GetBackMessageBlock(cGZMessage& msgOut);

	virtual int32_t GetMessageCount();

protected:
	typedef std::list<cGZMessage>::iterator MessageListIterator;
	typedef std::list<cGZMessage>::reverse_iterator MessageListReverseIterator;

	std::list<cGZMessage> messages;
	cRZCriticalSection criticalSection;
	cRZThreadSignal threadSignal;
	uint32_t refCount;
};