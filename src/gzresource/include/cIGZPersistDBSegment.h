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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistDBSegment = 0x656B8F0C;

struct cGZPersistResourceKey;
class cIGZPersistDBRecord;
class cIGZPersistResourceKeyFilter;
class cIGZPersistResourceKeyList;
class cIGZString;

class cIGZPersistDBSegment : public cIGZUnknown
{
public:
	virtual bool Init(void) = 0;
	virtual bool Shutdown(void) = 0;

	virtual bool Open(bool readable, bool writable) = 0;
	virtual bool IsOpen(void) const = 0;

	virtual bool Close(void) = 0;
	virtual bool Flush(void) = 0;

	virtual void GetPath(cIGZString& out) const = 0;
	virtual bool SetPath(cIGZString const& path) = 0;

	virtual bool Lock(void) = 0;
	virtual bool Unlock(void) = 0;

	virtual uint32_t GetSegmentID(void) const = 0;
	virtual bool SetSegmentID(uint32_t const& id) = 0;

	virtual uint32_t GetRecordCount(cIGZPersistResourceKeyFilter* filter) = 0;
	virtual uint32_t GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter) = 0;
	virtual uint32_t GetResourceKeyList(cIGZPersistResourceKeyList& list) = 0;

	virtual bool TestForRecord(cGZPersistResourceKey const& key) = 0;
	virtual int32_t GetRecordSize(cGZPersistResourceKey const& key) = 0;

	virtual bool OpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out, uint32_t) = 0;
	virtual bool CreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out) = 0;

	virtual bool CloseRecord(cIGZPersistDBRecord* record) = 0;
	virtual bool CloseRecord(cIGZPersistDBRecord** record) = 0;

	virtual bool AbortRecord(cIGZPersistDBRecord* record) = 0;
	virtual bool AbortRecord(cIGZPersistDBRecord** record) = 0;

	virtual bool DeleteRecord(cGZPersistResourceKey const& key) = 0;
	virtual uint32_t ReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length) = 0;
	virtual bool WriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length) = 0;

	virtual bool Init(uint32_t segmentID, cIGZString const* path, bool allowOpen) = 0;
};