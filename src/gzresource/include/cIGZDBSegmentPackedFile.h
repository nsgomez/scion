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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZDBSegmentPackedFile = 0x286D4A6A;

struct cGZPersistResourceKey;
class cIGZFile;
class cIGZPersistDBSegment;
class cIGZPersistResourceKeyFilter;
class cIGZString;

class cIGZDBSegmentPackedFile : public cIGZUnknown
{
public:
	virtual cIGZPersistDBSegment* AsIGZPersistDBSegment(void) = 0;

	virtual bool Init(void) = 0;
	virtual bool Shutdown(void) = 0;

	virtual uint32_t GetFileStartingOffset(void) const = 0;
	virtual bool SetFileStartingOffset(uint32_t offset) = 0;

	virtual void GetPath(cIGZString& path) const = 0;
	virtual bool SetPath(cIGZString const& path) = 0;

	virtual void GetCurrentUserVersion(uint32_t& major, uint32_t& minor) = 0;
	virtual void GetNextUserVersion(uint32_t& major, uint32_t& minor) = 0;
	virtual void SetNextUserVersion(uint32_t major, uint32_t minor) = 0;

	virtual bool SetFileBuffering(uint32_t readBufferSize, uint32_t writeBufferSize) = 0;
	virtual bool SetHoleUseFlags(int32_t flags) = 0;
	virtual void SetHoleCompactionMinimumRatio(float ratio) = 0;

	virtual bool IsRecordOpen(cGZPersistResourceKey const& key) = 0;
	virtual bool GetRecordAsIGZFile(cIGZPersistDBRecord* record, cIGZFile** fileOut) = 0;
	virtual uint32_t GetRecordSizeRaw(cGZPersistResourceKey const& key) = 0;
	virtual uint32_t ReadRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t&) = 0;
	virtual bool WriteRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t size) = 0;
	virtual uint32_t GetRecordPosition(cGZPersistResourceKey const& key) = 0;

	virtual uint32_t GetUsedTypeCount(void) = 0;
	virtual uint32_t GetUsedGroupCount(void) = 0;
	virtual void EnableUsedTypeAndGroupCounting(bool enabled) = 0;

	virtual uint32_t CopyDatabaseRecords(cIGZPersistDBSegment* segment, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord) = 0;
	virtual uint32_t CopyDatabaseRecords(cIGZString const& path, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord) = 0;

	virtual bool CompactDatabase(void) = 0;
	virtual bool VerifyDatabase(void) = 0;

	virtual bool EnableCompressionForAllRecords(bool flag) = 0;
	virtual bool EnableCompressionByType(uint32_t type, bool flag) = 0;
	virtual bool EnableCompressionByGroup(uint32_t group, bool flag) = 0;
	virtual bool GetRecordCompressed(cGZPersistResourceKey const& key) = 0;
	virtual void SetRecordCompressed(cGZPersistResourceKey const& key, bool flag, uint32_t size) = 0;

	virtual bool AllocateSpace(uint32_t& offset, uint32_t sizeToAlloc) = 0;
	virtual bool FreeSpace(uint32_t holeStart, uint32_t holeSize) = 0;
	virtual bool ReallocateSpace(cGZPersistResourceKey const& key, uint32_t&, uint32_t) = 0;
	virtual bool GetFreeSpaceMetrics(uint32_t& holeCount, uint32_t& totalHoleSize) = 0;
};