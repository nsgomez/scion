/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSegment.h"
#include "cRZAutoRefCount.h"

class cGZPersistDBRecord : public cIGZPersistDBRecord
{
public:
	cGZPersistDBRecord(cIGZPersistDBSegment* segment);
	cGZPersistDBRecord(cGZPersistResourceKey const& key, cIGZPersistDBSegment* segment);
	virtual ~cGZPersistDBRecord() { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual cGZPersistResourceKey GetKey(void);
	virtual int32_t GetAccessFlags(void);

	virtual bool Close(void);

public:
	virtual void SetKey(cGZPersistResourceKey const& key);
	virtual void SetAccessFlags(int32_t flags);

protected:
	uint16_t refCount;
	uint16_t accessFlags;
	cGZPersistResourceKey key;
	cRZAutoRefCount<cIGZPersistDBSegment> segment;
};