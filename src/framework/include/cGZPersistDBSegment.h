/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cIGZPersistDBSegment.h"
#include "cRZCriticalSection.h"

class cGZPersistDBSegment : public cIGZPersistDBSegment
{
public:
	cGZPersistDBSegment(void);
	virtual ~cGZPersistDBSegment(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual bool Lock(void);
	virtual bool Unlock(void);

	virtual bool GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter);
	virtual bool GetResourceKeyList(cIGZPersistResourceKeyList& list);

	virtual bool TestForRecord(cGZPersistResourceKey const& key);
	virtual int32_t GetRecordSize(cGZPersistResourceKey const& key);

	virtual bool OpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out, uint32_t);
	virtual bool CreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out);

	virtual bool CloseRecord(cIGZPersistDBRecord* record);
	virtual bool CloseRecord(cIGZPersistDBRecord** recordPtr);

	virtual bool AbortRecord(cIGZPersistDBRecord* record);
	virtual bool AbortRecord(cIGZPersistDBRecord** recordPtr);

	virtual bool DeleteRecord(cGZPersistResourceKey const& key);
	virtual bool ReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length);
	virtual bool WriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length);

public:
	virtual void FinalRelease(void);

	virtual bool DoTestForRecord(cGZPersistResourceKey const& key) = 0;
	virtual int32_t DoGetRecordSize(cGZPersistResourceKey const& key) = 0;
	virtual bool DoOpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t flags) = 0;
	virtual bool DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr) = 0;
	virtual bool DoCloseRecord(cIGZPersistDBRecord* record) = 0;
	virtual bool DoAbortRecord(cIGZPersistDBRecord* recordPtr) = 0;
	virtual bool DoDeleteRecord(cGZPersistResourceKey const& key) = 0;
	virtual bool DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length) = 0;
	virtual bool DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length) = 0;

protected:
	uint32_t refCount;
	cRZCriticalSection criticalSection;
};