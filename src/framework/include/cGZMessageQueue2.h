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
#include "cIGZMessageQueue2.h"
#include "cRZAutoRefCount.h"
#include "cRZCriticalSection.h"
#include "cRZThreadSignal.h"

class cGZMessageQueue2 : public cIGZMessageQueue2
{
public:
	cGZMessageQueue2();
	virtual ~cGZMessageQueue2();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual void PushBack(cIGZMessage2* msg);
	virtual void PushFront(cIGZMessage2* msg);

	virtual bool RemoveFirstFound(cIGZMessage2 const& msg);
	virtual bool RemoveLastFound(cIGZMessage2 const& msg);
	virtual int32_t RemoveAllFound(cIGZMessage2 const& msg);

	virtual bool RemoveFirstFound(GZGUID msgType);
	virtual bool RemoveLastFound(GZGUID msgType);
	virtual int32_t RemoveAllFound(GZGUID msgType);

	virtual int32_t RemoveAll();

	virtual GZGUID PeekFrontType();
	virtual GZGUID PeekBackType();
	virtual bool PeekAnywhereType(GZGUID msgType);

	virtual bool PeekFrontMessage(cIGZMessage2** msgOut);
	virtual bool PeekBackMessage(cIGZMessage2** msgOut);
	virtual bool PeekAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut);

	virtual bool GetFrontMessage(cIGZMessage2** msgOut);
	virtual bool GetBackMessage(cIGZMessage2** msgOut);

	virtual bool GetFrontMessageBlock(cIGZMessage2** msgOut);
	virtual bool GetBackMessageBlock(cIGZMessage2** msgOut);

	virtual int32_t GetMessageCount();

public:
	virtual bool GetAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut);

protected:
	typedef std::list< cRZAutoRefCount<cIGZMessage2> >::iterator Message2ListIterator;
	typedef std::list< cRZAutoRefCount<cIGZMessage2> >::reverse_iterator Message2ListReverseIterator;

	std::list< cRZAutoRefCount<cIGZMessage2> > messages;
	cRZCriticalSection criticalSection;
	cRZThreadSignal threadSignal;
	uint32_t refCount;
};