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

#include <ctime>
#include <list>
#include <string>
#include "cGZDBSegmentPackedFile.h"
#include "cGZDBReadRecordPackedFile.h"
#include "cGZDBReadRecordRAM.h"
#include "cGZDBSerialRecordAsIGZFile.h"
#include "cGZDBWriteRecordPackedFile.h"
#include "cGZPersistDBSerialRecord.h"
#include "cGZPersistResourceKeyList.h"
#include "cIGZPersistResourceKeyFilter.h"
#include "cRZCriticalSection.h"
#include "cRZFastCompression3.h"

static const uint32_t TypeID_PackedFileCompressedSet = 0xE86B1EEF;
static const uint32_t InstanceID_PackedFileCompressedSet = 0x286B1F03;

cGZDBSegmentPackedFile::cGZDBSegmentPackedFile(void)
{
	// TODO
}

cGZDBSegmentPackedFile::~cGZDBSegmentPackedFile(void)
{
	Shutdown();
}

bool cGZDBSegmentPackedFile::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZDBSegmentPackedFile)
	{
		*outPtr = static_cast<cIGZDBSegmentPackedFile*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cGZPersistDBSegment::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZDBSegmentPackedFile::AddRef(void)
{
	return cGZPersistDBSegment::AddRef();
}

uint32_t cGZDBSegmentPackedFile::Release(void)
{
	return cGZPersistDBSegment::Release();
}

cIGZPersistDBSegment* cGZDBSegmentPackedFile::AsIGZPersistDBSegment(void)
{
	return static_cast<cIGZPersistDBSegment*>(this);
}

bool cGZDBSegmentPackedFile::Init(void)
{
	if (!inited)
	{
		inited = true;
	}

	return true;
}

bool cGZDBSegmentPackedFile::Shutdown(void)
{
	if (inited)
	{
		inited = false;
		if (IsOpenInternal())
		{
		}

		if (smallScratchBuffer != NULL)
		{
			delete smallScratchBuffer;
		}

		smallScratchBuffer = NULL;
	}

	return true;
}

uint32_t cGZDBSegmentPackedFile::GetFileStartingOffset(void) const
{
	return startingOffset;
}

bool cGZDBSegmentPackedFile::SetFileStartingOffset(uint32_t offset)
{
	if (!IsOpenInternal())
	{
		startingOffset = offset;
		return true;
	}

	return false;
}

void cGZDBSegmentPackedFile::GetPath(cIGZString& path) const
{
	path.FromChar(this->path.ToChar());
}

bool cGZDBSegmentPackedFile::SetPath(cIGZString const& path)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		this->path = path;
		return true;
	}

	return false;
}

void cGZDBSegmentPackedFile::GetCurrentUserVersion(uint32_t& major, uint32_t& minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	major = header.currentUserVersionMajor;
	minor = header.currentUserVersionMinor;
}

void cGZDBSegmentPackedFile::GetNextUserVersion(uint32_t& major, uint32_t& minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	major = nextUserVersionMajor;
	minor = nextUserVersionMinor;
}

void cGZDBSegmentPackedFile::SetNextUserVersion(uint32_t major, uint32_t minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	nextUserVersionMajor = major;
	nextUserVersionMinor = minor;
}

bool cGZDBSegmentPackedFile::SetFileBuffering(uint32_t readBufferSize, uint32_t writeBufferSize)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (file.SetBuffering(readBufferSize, writeBufferSize))
	{
		this->readBufferSize = readBufferSize;
		this->writeBufferSize = writeBufferSize;
	}

	return true;
}

bool cGZDBSegmentPackedFile::SetHoleUseFlags(int32_t flags)
{
	cRZCriticalSectionHolder lock(criticalSection);
	holeUseFlags = flags;
	return true;
}

void cGZDBSegmentPackedFile::SetHoleCompactionMinimumRatio(float ratio)
{
	holeCompactionMinRatio = ratio;
}

bool cGZDBSegmentPackedFile::IsRecordOpen(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return openRecords.find(key) != openRecords.end();
}

bool cGZDBSegmentPackedFile::GetRecordAsIGZFile(cIGZPersistDBRecord* record, cIGZFile** fileOut)
{
	cRZCriticalSectionHolder lock(criticalSection);
	cIGZFile* wrapper = static_cast<cIGZFile*>(new cGZDBSerialRecordAsIGZFile(
		// FUTURE: this really is what the game does but this is wildly unsafe
		static_cast<cGZPersistDBSerialRecord*>(record)));

	*fileOut = wrapper;
	wrapper->AddRef();
	return true;
}

uint32_t cGZDBSegmentPackedFile::GetRecordSizeRaw(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t size = -1;

	if (IsOpenInternal())
	{
		tRecordInfoTable::iterator it = recordInfoTable->find(key);
		if (it != recordInfoTable->end())
		{
			return it->second.size;
		}
	}

	return size;
}

uint32_t cGZDBSegmentPackedFile::ReadRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t& bytesRead)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		return -1;
	}

	if (fileAccessFlags & GZFileAccessWrite)
	{
		tOpenRecordMap::iterator it = openRecords.find(key);
		if (it != openRecords.end() && (it->second->GetAccessFlags() & GZFileAccessWrite) != 0)
		{
			return -1;
		}
	}

	tRecordInfoTable::iterator it = recordInfoTable->find(key);
	if (it == recordInfoTable->end())
	{
		return -1;
	}
	else if (data == NULL)
	{
		bytesRead = 0;
		tCompressedRecordsMap::iterator compressedRec = compressedRecords.find(key);
		return (compressedRec == compressedRecords.end())
			? it->second.size
			: compressedRec->second.uncompressedSize;
	}
	else
	{
		uint32_t bytesToRead = bytesRead;
		if (it->second.size < bytesRead)
		{
			bytesToRead = it->second.size;
		}

		bytesRead = 0;
		if (file.SeekToPosition(startingOffset + it->second.offset) == startingOffset + it->second.offset
			&& file.Read(data, bytesToRead))
		{
			bytesRead = bytesToRead;
			return bytesToRead;
		}
		else
		{
			return 0;
		}
	}
}

bool cGZDBSegmentPackedFile::WriteRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t size)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator serialIt = openRecords.find(key);
	if (serialIt != openRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt != recordInfoTable->end())
	{
		IncrementTypeAndGroupUse(key.type, key.group, false);
		SetRecordCompressed(key, false, 0);
		recordInfoTable->erase(infoIt);
		FreeSpace(infoIt->second.offset, infoIt->second.size);
		shouldWriteRecords = true;
	}

	uint32_t offset = 0;
	if (AllocateSpace(offset, size))
	{
		RecordDataInfo newRecord(key, offset, size);
		if (WriteFileSpan(data, offset, size))
		{
			recordInfoTable->insert(std::pair<const KeyTGI, RecordDataInfo>(key, newRecord));
			IncrementTypeAndGroupUse(key.type, key.group, true);
			shouldWriteRecords = true;
			return true;
		}
		else
		{
			FreeSpace(offset, size);
		}
	}

	return false;
}

uint32_t cGZDBSegmentPackedFile::GetRecordPosition(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (IsOpenInternal())
	{
		tRecordInfoTable::iterator it = recordInfoTable->find(key);
		if (it != recordInfoTable->end())
		{
			return it->second.offset;
		}
	}

	return -1;
}

uint32_t cGZDBSegmentPackedFile::GetUsedTypeCount(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count = typeUseCount.size();

	if (count == 0)
	{
		std::hash_set<uint32_t> set;
		for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
		{
			set.insert(it->second.key.type);
		}

		for (tOpenRecordMap::iterator it = openRecords.begin(); it != openRecords.end(); it++)
		{
			set.insert(it->first.type);
		}

		count = set.size();
	}

	return count;
}

uint32_t cGZDBSegmentPackedFile::GetUsedGroupCount(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count = groupUseCount.size();

	if (count == 0)
	{
		std::hash_set<uint32_t> set;
		for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
		{
			set.insert(it->second.key.group);
		}

		for (tOpenRecordMap::iterator it = openRecords.begin(); it != openRecords.end(); it++)
		{
			set.insert(it->first.group);
		}

		count = set.size();
	}

	return count;
}

void cGZDBSegmentPackedFile::EnableUsedTypeAndGroupCounting(bool enabled)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (typeAndGroupUseTrackingEnabled != enabled)
	{
		typeAndGroupUseTrackingEnabled = enabled;

		typeUseCount.clear();
		groupUseCount.clear();

		if (typeAndGroupUseTrackingEnabled)
		{
			for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
			{
				IncrementTypeAndGroupUse(it->second.key.type, it->second.key.group, true);
			}
		}
	}
}

uint32_t cGZDBSegmentPackedFile::CopyDatabaseRecords(cIGZPersistDBSegment* segment, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord)
{
	cRZCriticalSectionHolder lock(criticalSection);

	Lock();
	segment->Lock();

	cGZPersistResourceKeyList* keyList = new cGZPersistResourceKeyList();
	keyList->AddRef();

	if (copyFromSegmentToSelf)
	{
		segment->GetResourceKeyList(keyList, filter);
	}
	else
	{
		this->GetResourceKeyList(keyList, filter);
	}

	std::deque<cGZPersistResourceKey>::iterator it = keyList->keys.begin();
	uint32_t recordsCopied = 0;

	while (it != keyList->keys.end())
	{
		if (CopyDatabaseRecord(segment, *it, copyFromSegmentToSelf, skipDeletingSourceRecord))
		{
			++recordsCopied;
		}

		++it;
	}

	keyList->Release();
	segment->Unlock();
	Unlock();

	return recordsCopied;
}

uint32_t cGZDBSegmentPackedFile::CopyDatabaseRecords(cIGZString const& path, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord)
{
	cRZCriticalSectionHolder lock(criticalSection);

	cGZDBSegmentPackedFile* otherFile = new cGZDBSegmentPackedFile();
	otherFile->AddRef();

	uint32_t recordsCopied = -1;
	if (otherFile->Init())
	{
		if (otherFile->SetPath(path) && otherFile->Open(true, !copyFromSegmentToSelf || !skipDeletingSourceRecord))
		{
			recordsCopied = CopyDatabaseRecords(otherFile, filter, copyFromSegmentToSelf, skipDeletingSourceRecord);
			otherFile->Close();
		}

		otherFile->Shutdown();
	}

	otherFile->Release();
	return recordsCopied;
}

bool cGZDBSegmentPackedFile::CompactDatabase(void)
{
	// TODO
	return false;
}

bool cGZDBSegmentPackedFile::VerifyDatabase(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return IsOpenInternal();
}

bool cGZDBSegmentPackedFile::EnableCompressionForAllRecords(bool flag)
{
	cRZCriticalSectionHolder lock(criticalSection);
	compressAllRecords = flag;
	return true;
}

bool cGZDBSegmentPackedFile::EnableCompressionByType(uint32_t type, bool flag)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (flag)
	{
		compressedTypeIDs.insert(type);
	}
	else
	{
		compressedTypeIDs.erase(type);
	}

	return false;
}

bool cGZDBSegmentPackedFile::EnableCompressionByGroup(uint32_t group, bool flag)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (flag)
	{
		compressedGroupIDs.insert(group);
	}
	else
	{
		compressedGroupIDs.erase(group);
	}

	return false;
}

bool cGZDBSegmentPackedFile::GetRecordCompressed(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return compressedRecords.find(key) != compressedRecords.end();
}

void cGZDBSegmentPackedFile::SetRecordCompressed(cGZPersistResourceKey const& key, bool flag, uint32_t size)
{
	cRZCriticalSectionHolder lock(criticalSection);
	tCompressedRecordsMap::iterator it = compressedRecords.find(key);
	if (flag)
	{
		if (it == compressedRecords.end())
		{
			CompressedRecordData record(key, size);
			compressedRecords.insert(std::pair<cGZPersistResourceKey, CompressedRecordData>(key, record));
			shouldWriteCompressedRecords = true;
		}
		else
		{
			it->second.uncompressedSize = size;
		}
	}
	else if (it != compressedRecords.end())
	{
		compressedRecords.erase(it);
		shouldWriteCompressedRecords = true;
	}
}

bool cGZDBSegmentPackedFile::AllocateSpace(uint32_t& offset, uint32_t sizeToAlloc)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	uint32_t origFileSize = file.Length();
	if (holeUseFlags & DBPFEnableHoleReuse)
	{
		for (tHoleSet::iterator it = holes.begin(); it != holes.end(); it++)
		{
			if (sizeToAlloc <= it->size)
			{
				uint32_t holeOffset = it->offset;
				uint32_t holeSize = it->size;

				offset = holeOffset;
				holes.erase(it);

				FreeSpace(sizeToAlloc + holeOffset, holeSize - sizeToAlloc);
				return true;
			}
		}
	}

	if (file.SetLength(origFileSize + sizeToAlloc))
	{
		offset = origFileSize;
		return true;
	}

	return false;
}

bool cGZDBSegmentPackedFile::FreeSpace(uint32_t holeStart, uint32_t holeSize)
{
	if (holeStart != 0 && holeSize != 0)
	{
		holes.insert(HoleRecord(holeStart, holeSize));
		shouldWriteHoles = true;
	}

	return true;
}

bool cGZDBSegmentPackedFile::ReallocateSpace(cGZPersistResourceKey const& key, uint32_t&, uint32_t)
{
	return false;
}

bool cGZDBSegmentPackedFile::GetFreeSpaceMetrics(uint32_t& holeCount, uint32_t& totalHoleSize)
{
	holeCount = 0;
	totalHoleSize = 0;

	if (!IsOpenInternal())
	{
		return false;
	}

	for (tHoleSet::iterator it = holes.begin(); it != holes.end(); it++)
	{
		++holeCount;
		totalHoleSize += it->size;
	}

	return true;
}

bool cGZDBSegmentPackedFile::Open(bool readable, bool writable)
{
	if (IsOpenInternal())
	{
		return false;
	}

	if (path.Strlen() == 0)
	{
		return false;
	}

	if (!inited)
	{
		return false;
	}

	if (!readable && !writable)
	{
		return false;
	}

	uint32_t accessFlags = GZFileAccessReadWrite;
	if (!writable)
	{
		accessFlags = GZFileAccessRead;
	}

	fileAccessFlags = accessFlags;
	file.SetPath(path);

	if (!file.Open(fileAccessFlags, (writable ? GZFileCreateNewOpenExisting : GZFileOpenExistingOnly), GZFileShareRead))
	{
		fileAccessFlags = 0;
		return false;
	}

	if (file.Length() != 0 || ((fileAccessFlags & GZFileAccessWrite) != 0 && WriteEmptyHeaderRecord()))
	{
		if (!ReadHeaderRecord())
		{
			if ((fileAccessFlags & GZFileAccessWrite) != 0 && WriteEmptyHeaderRecord())
			{
				if (recordInfoTable == NULL)
				{
					recordInfoTable = new tRecordInfoTable();
				}

				return true;
			}
		}
		else
		{
			if (ReadIndexRecord() && ReadHoleRecord() && ReadCompressedSetRecord())
			{
				return true;
			}
		}
	}

	file.Close();
	return false;
}

bool cGZDBSegmentPackedFile::IsOpen(void) const
{
	return IsOpenInternal();
}

bool cGZDBSegmentPackedFile::Close(void)
{
	if (!IsOpenInternal())
	{
		return true;
	}

	bool succeeded = true;
	if (fileAccessFlags & GZFileAccessWrite)
	{
		succeeded = CloseOpenRecords() && Flush();
	}

	delete recordInfoTable;
	recordInfoTable = NULL;

	typeUseCount.clear();
	groupUseCount.clear();
	holes.clear();
	compressedRecords.clear();

	file.Close();
	fileAccessFlags = 0;

	return succeeded;
}

bool cGZDBSegmentPackedFile::Flush(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!WriteCompressedSetRecord())
	{
		return false;
	}

	if ((holeUseFlags & DBPFEnableHoleCompaction) != 0 && ShouldCompactDatabase())
	{
		CompactDatabase();
	}

	return WriteIndexRecord()
		&& WriteHoleRecord()
		&& WriteHeaderRecord()
		&& file.Flush();
}

uint32_t cGZDBSegmentPackedFile::GetSegmentID(void) const
{
	return segmentID;
}

bool cGZDBSegmentPackedFile::SetSegmentID(uint32_t const& id)
{
	segmentID = id;
	return true;
}

uint32_t cGZDBSegmentPackedFile::GetRecordCount(cIGZPersistResourceKeyFilter* filter)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (filter == NULL)
	{
		if (recordInfoTable == NULL)
		{
			return 0;
		}
		else
		{
			return recordInfoTable->size();
		}
	}
	else
	{
		return GetResourceKeyList(NULL, filter);
	}
}

uint32_t cGZDBSegmentPackedFile::GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter)
{
	uint32_t count = 0;
	if (IsOpenInternal())
	{
		for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
		{
			cGZPersistResourceKey key = it->second;
			if (it->second.key.type != TypeID_PackedFileCompressedSet)
			{
				if (filter != NULL && !filter->IsKeyIncluded(key))
				{
					continue;
				}

				if (list != NULL)
				{
					list->Insert(key);
				}

				++count;
			}
		}
	}

	return count;
}

bool cGZDBSegmentPackedFile::Init(uint32_t segmentID, cIGZString const* path, bool allowOpen)
{
	if (!inited)
	{
		inited = true;
		if (path != NULL)
		{
			this->path = *path;
		}

		this->segmentID = segmentID;
	}

	return true;
}

void cGZDBSegmentPackedFile::FinalRelease(void)
{
	if (IsOpenInternal())
	{
		Close();
	}

	if (inited)
	{
		Shutdown();
	}

	cGZPersistDBSegment::FinalRelease();
}

bool cGZDBSegmentPackedFile::DoTestForRecord(cGZPersistResourceKey const& key)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	return recordInfoTable->find(key) != recordInfoTable->end()
		|| openRecords.find(key) != openRecords.end();
}

bool cGZDBSegmentPackedFile::DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator openRecordIt = openRecords.find(key);
	if (openRecordIt != openRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator recInfo = recordInfoTable->find(key);
	if (recInfo == recordInfoTable->end())
	{
		return DoCreateNewRecord(key, outPtr);
	}

	//cGZDBWriteRecordPackedFile* record;

	// TODO
	if (compressedRecords.find(key) == compressedRecords.end())
	{
	}
	else
	{
	}

	// TODO
	return true;
}

int32_t cGZDBSegmentPackedFile::DoGetRecordSize(cGZPersistResourceKey const& key)
{
	if (!IsOpenInternal())
	{
		return -1;
	}

	if (fileAccessFlags & GZFileAccessWrite)
	{
		tOpenRecordMap::iterator it = openRecords.find(key);
		if (it != openRecords.end())
		{
			size_t size = it->second->GetSize();
			if (size != -1)
			{
				return -1;
			}
		}
	}

	if (GetRecordCompressed(key))
	{
		tCompressedRecordsMap::iterator it = compressedRecords.find(key);
		if (it != compressedRecords.end())
		{
			return it->second.uncompressedSize;
		}
	}
	else
	{
		tRecordInfoTable::iterator it = recordInfoTable->find(key);
		if (it != recordInfoTable->end())
		{
			return it->second.size;
		}
	}

	return -1;
}

bool cGZDBSegmentPackedFile::DoOpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t flags)
{
	if (!IsOpenInternal())
	{
		*outPtr = NULL;
		return false;
	}

	if (flags & GZFileAccessWrite)
	{
		return DoOpenRecordForWrite(key, outPtr);
	}

	if ((flags & GZFileAccessRead) == 0)
	{
		*outPtr = NULL;
		return false;
	}

	tOpenRecordMap::iterator openRecIt = openRecords.find(key);
	cGZPersistDBRecord* record;

	if (openRecIt == openRecords.end())
	{
		tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
		if (infoIt == recordInfoTable->end())
		{
			*outPtr = NULL;
			return false;
		}

		tCompressedRecordsMap::iterator compressedIt = compressedRecords.find(key);
		cGZPersistDBSerialRecord* serialRecord;

		if (compressedIt == compressedRecords.end())
		{
			if (infoIt->second.size <= sizeLimitForInMemoryRecords)
			{
				uint8_t* memory = new uint8_t[infoIt->second.size];
				if (!ReadFileSpan(memory, infoIt->second.offset, infoIt->second.size))
				{
					delete[] memory;
					*outPtr = NULL;
					return false;
				}

				cGZDBReadRecordRAM* ramRecord = new cGZDBReadRecordRAM(memory, infoIt->second.size, key, this, true);
				serialRecord = ramRecord;
			}
			else
			{
				cGZDBReadRecordPackedFile* fileRecord = new cGZDBReadRecordPackedFile(infoIt->second.offset, infoIt->second.size, key, this, true);
				serialRecord = fileRecord;
			}
		}
		else
		{
			uint8_t* decompressedData;
			uint32_t decompressedSize;

			if (!DecompressRecord(infoIt->second.offset, infoIt->second.size, decompressedData, decompressedSize))
			{
				*outPtr = NULL;
				return false;
			}

			serialRecord = new cGZDBReadRecordRAM(decompressedData, decompressedSize, key, this, true);
		}

		record = static_cast<cGZPersistDBRecord*>(serialRecord);
		record->AddRef();
	}
	else
	{
		cGZPersistDBSerialRecord* openRecord = openRecIt->second;
		if ((openRecord->GetAccessFlags() & GZFileAccessWrite) != 0)
		{
			*outPtr = NULL;
			return false;
		}

		cGZDBRecord* readRecord = static_cast<cGZDBRecord*>(openRecord);
		uint32_t recordType = readRecord->GetReadRecordType();

		if (recordType == DBReadRecordTypeRAM)
		{
			cGZDBReadRecordRAM* original = static_cast<cGZDBReadRecordRAM*>(readRecord);
			record = new cGZDBReadRecordRAM(original->data, original->size, key, this, true);
		}
		else if (recordType == DBReadRecordTypePackedFile)
		{
			cGZDBReadRecordPackedFile* original = static_cast<cGZDBReadRecordPackedFile*>(readRecord);
			record = new cGZDBReadRecordPackedFile(original->recordOffset, original->size, key, this, true);
		}
		else
		{
			*outPtr = NULL;
			return false;
		}

		record->AddRef();
	}

	if (record != NULL)
	{
		cGZPersistDBSerialRecord* serialRecord = static_cast<cGZPersistDBSerialRecord*>(record);
		openRecords.insert(std::pair<cGZPersistResourceKey, cGZPersistDBSerialRecord*>(key, serialRecord));
	}

	*outPtr = record;
	return record != NULL;
}

bool cGZDBSegmentPackedFile::DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt != recordInfoTable->end())
	{
		return false;
	}

	tOpenRecordMap::iterator openIt = openRecords.find(key);
	if (openIt != openRecords.end())
	{
		return false;
	}

	cGZDBWriteRecordPackedFile* newRecord = new cGZDBWriteRecordPackedFile(0U, 0U, key, this, true);
	newRecord->AddRef();

	openRecords.insert(std::pair<cGZPersistResourceKey, cGZPersistDBSerialRecord*>(key, newRecord));
	return true;
}

bool cGZDBSegmentPackedFile::DoCloseRecord(cIGZPersistDBRecord* record)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	cGZPersistResourceKey key = record->GetKey();
	for (tOpenRecordMap::iterator it = openRecords.find(key); it != openRecords.end() && it->first == key; it++)
	{
		cGZPersistDBSerialRecord* serialRecord = it->second;
		if (static_cast<cGZPersistDBRecord*>(serialRecord) == record)
		{
			if (record->GetAccessFlags() & GZFileAccessWrite)
			{
				WriteRecordInternal(serialRecord, key);
			}

			static_cast<cGZDBRecord*>(record)->DoPostClose();
			serialRecord->Release();

			openRecords.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::DoAbortRecord(cIGZPersistDBRecord* record)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	cGZPersistResourceKey key = record->GetKey();
	for (tOpenRecordMap::iterator it = openRecords.find(key); it != openRecords.end() && it->first == key; it++)
	{
		cGZPersistDBSerialRecord* serialRecord = it->second;
		if (static_cast<cGZPersistDBRecord*>(serialRecord) == record)
		{
			static_cast<cGZDBRecord*>(record)->DoPostClose();
			serialRecord->Release();

			openRecords.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::DoDeleteRecord(cGZPersistResourceKey const& key)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator openIt = openRecords.find(key);
	if (openIt == openRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt == recordInfoTable->end())
	{
		return false;
	}

	IncrementTypeAndGroupUse(key.type, key.group, false);

	SetRecordCompressed(key, false, 0);
	recordInfoTable->erase(infoIt);
	FreeSpace(infoIt->second.offset, infoIt->second.size);

	shouldWriteRecords = true;
	return true;
}

uint32_t cGZDBSegmentPackedFile::DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
{
	if ((fileAccessFlags & GZFileAccessRead) != 0)
	{
		length = 0;
		return -1;
	}

	tOpenRecordMap::iterator openRecIt;
	if ((fileAccessFlags & GZFileAccessWrite) != 0
		&& (openRecIt = openRecords.find(key)) != openRecords.end()
		&& (openRecIt->second->GetAccessFlags() & GZFileAccessWrite) != 0)
	{
		length = 0;
		return -1;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt == recordInfoTable->end())
	{
		length = 0;
		return -1;
	}

	tCompressedRecordsMap::iterator compressedIt = compressedRecords.find(key);
	if (data != NULL)
	{
		if (compressedIt == compressedRecords.end())
		{
			uint32_t bytesToRead = length;
			if (infoIt->second.size < bytesToRead)
			{
				bytesToRead = infoIt->second.size;
			}

			if (file.SeekToPosition(startingOffset + infoIt->second.offset) != startingOffset + infoIt->second.offset
				|| !file.Read(data, bytesToRead))
			{
				length = 0;
				return 0;
			}

			length = bytesToRead;
			return bytesToRead;
		}
		else
		{
			uint8_t* decompressedData;
			if (DecompressRecord(key, decompressedData, length))
			{
				return length;
			}

			length = 0;
			return 0;
		}
	}
	else
	{
		length = 0;
		if (compressedIt == compressedRecords.end())
		{
			return infoIt->second.size;
		}
		else
		{
			return compressedIt->second.uncompressedSize;
		}
	}
}

bool cGZDBSegmentPackedFile::DoReadRecord(cGZPersistResourceKey const& key, cIGZString const& filename)
{
	// TODO
	return false;
}

bool cGZDBSegmentPackedFile::DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length)
{
	tOpenRecordMap::iterator openRecIt;
	if ((fileAccessFlags & GZFileAccessWrite) == 0
		|| (openRecIt = openRecords.find(key)) != openRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt != recordInfoTable->end())
	{
		uint32_t offset = infoIt->second.offset;
		uint32_t size = infoIt->second.size;

		IncrementTypeAndGroupUse(key.type, key.group, false);
		SetRecordCompressed(key, false, 0);

		recordInfoTable->erase(infoIt);
		FreeSpace(offset, size);
		shouldWriteRecords = true;
	}

	bool usingOriginalDataPtr = true;
	if (ShouldRecordBeCompressed(key, length))
	{
		uint8_t* compressedData;
		uint32_t compressedSize;

		if (ShouldRecordBeCompressed(reinterpret_cast<uint8_t*>(data), length, compressedData, compressedSize))
		{
			SetRecordCompressed(key, true, length);

			usingOriginalDataPtr = false;
			data = compressedData;
			length = compressedSize;
		}
	}

	uint32_t offset = 0;
	bool result;

	if (AllocateSpace(offset, length))
	{
		RecordDataInfo record(key, offset, length);
		if (WriteFileSpan(data, offset, length))
		{
			recordInfoTable->insert(std::pair<KeyTGI, RecordDataInfo>(key, record));
			IncrementTypeAndGroupUse(key.type, key.group, true);
			shouldWriteRecords = true;
		}
		else
		{
			FreeSpace(offset, length);
		}
	}
	else
	{
		result = false;
	}

	if (usingOriginalDataPtr || data == NULL)
	{
		return result;
	}

	delete[] data;
	return result;
}

bool cGZDBSegmentPackedFile::DoWriteRecord(cGZPersistResourceKey const& key, cIGZString const& filename)
{
	// TODO
	return false;
}

bool cGZDBSegmentPackedFile::GetModificationTime(uint32_t& creationTime, uint32_t& modificationTime)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (IsOpenInternal())
	{
		creationTime = header.timestampCreated;
		modificationTime = header.timestampModified;
		return true;
	}

	return false;
}

bool cGZDBSegmentPackedFile::SeekAbsolute(int32_t position)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		return false;
	}

	return file.SeekToPosition(startingOffset + position) == startingOffset + position;
}

uint32_t cGZDBSegmentPackedFile::ReadFileSpan(void* data, uint32_t offset, uint32_t length)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		return false;
	}

	if (file.SeekToPosition(startingOffset + offset) != startingOffset + offset)
	{
		return false;
	}

	return file.Read(data, length);
}

bool cGZDBSegmentPackedFile::ReadFileSpanWithCount(void* data, uint32_t offset, uint32_t& length)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		return false;
	}

	if (file.SeekToPosition(startingOffset + offset) != startingOffset + offset)
	{
		return false;
	}

	return file.ReadWithCount(data, length);
}

bool cGZDBSegmentPackedFile::WriteFileSpan(void const* data, uint32_t offset, uint32_t length)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	if (file.SeekToPosition(startingOffset + offset) != startingOffset + offset)
	{
		return false;
	}

	return file.Write(data, length);
}

uint32_t cGZDBSegmentPackedFile::GetDecompressedRecordLength(cGZPersistResourceKey const& key)
{
	tCompressedRecordsMap::iterator it = compressedRecords.find(key);
	if (it != compressedRecords.end())
	{
		return it->second.uncompressedSize;
	}

	return -1;
}

bool cGZDBSegmentPackedFile::WriteEmptyHeaderRecord(void)
{
	memset(&header, 0, sizeof(header));

	header.signature = kPackedFileHeaderFileIdentifier;
	header.majorVersion = kPackedFileHeaderMajorVersion;
	header.minorVersion = kPackedFileHeaderMinorVersion;
	header.currentUserVersionMajor = nextUserVersionMajor;
	header.currentUserVersionMinor = nextUserVersionMinor;

	header.__reserved0 = 0;

	header.timestampCreated = time(NULL);
	header.timestampModified = time(NULL);

	header.indexMajorVersion = 7;
	header.indexEntryCount = 0;
	header.indexRecordOffset = 0;
	header.indexTableSize = 0;

	header.holeEntryCount = 0;
	header.holeRecordOffset = 0;
	header.holeRecordSize = 0;

	return WriteHeaderRecord() && file.SetLength(sizeof(header));
}

bool cGZDBSegmentPackedFile::WriteHeaderRecord(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0
		|| !file.IsOpen()
		|| file.SeekToPosition(startingOffset) != startingOffset)
	{
		return false;
	}

	DBPFHeader writeHeader;
	memcpy(&writeHeader, &this->header, sizeof(writeHeader));

	writeHeader.signature = kPackedFileHeaderFileIdentifier;
	writeHeader.majorVersion = kPackedFileHeaderMajorVersion;
	writeHeader.minorVersion = kPackedFileHeaderMinorVersion;
	writeHeader.currentUserVersionMinor = nextUserVersionMinor;
	writeHeader.timestampModified = time(NULL);

	return file.Write(&writeHeader, sizeof(writeHeader));
}

bool cGZDBSegmentPackedFile::WriteIndexRecord(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!shouldWriteRecords)
	{
		return true;
	}

	uint32_t originalIndexSize = header.indexTableSize;
	uint32_t newIndexEntryCount = recordInfoTable->size();
	uint32_t newIndexSize = newIndexEntryCount * sizeof(RecordDataInfo);

	if (newIndexSize == 0)
	{
		bool success = FreeSpace(header.indexRecordOffset, originalIndexSize);
		shouldWriteRecords = false;
		return success;
	}

	header.indexEntryCount = newIndexEntryCount;
	header.indexTableSize = newIndexSize;

	if (newIndexSize - originalIndexSize != 0)
	{
		if (!FreeSpace(header.indexRecordOffset, originalIndexSize))
		{
			shouldWriteRecords = false;
			return false;
		}

		header.indexRecordOffset = 0;

		if (!AllocateSpace(header.indexRecordOffset, header.indexTableSize))
		{
			header.indexEntryCount = 0;
			header.indexRecordOffset = 0;
			header.indexTableSize = 0;

			shouldWriteRecords = false;
			return false;
		}
	}

	RecordDataInfo* recordTable = new RecordDataInfo[newIndexEntryCount];
	RecordDataInfo* cursor = recordTable;

	for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
	{
		*(cursor++) = it->second;
	}

	bool success = WriteFileSpan(recordTable, header.indexRecordOffset, header.indexTableSize);
	delete[] recordTable;

	if (!success && header.indexRecordOffset != 0)
	{
		FreeSpace(header.indexRecordOffset, header.indexTableSize);

		header.indexEntryCount = 0;
		header.indexRecordOffset = 0;
		header.indexTableSize = 0;
	}

	shouldWriteRecords = false;
	return success;
}

bool cGZDBSegmentPackedFile::WriteHoleRecord(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!shouldWriteHoles)
	{
		return true;
	}

	uint32_t newHoleCount = holes.size();
	if (newHoleCount != header.holeEntryCount)
	{
		bool success = FreeSpace(header.holeRecordOffset, header.holeRecordSize);
		if (success && holes.size() != 0)
		{
			uint32_t newHoleRecordSize = holes.size() * sizeof(HoleRecord);
			header.holeRecordSize = newHoleRecordSize;

			success = AllocateSpace(header.holeRecordOffset, newHoleRecordSize);
		}

		if (!success)
		{
			header.holeEntryCount = 0;
			header.holeRecordOffset = 0;
			header.holeRecordSize = 0;

			shouldWriteHoles = false;
			return false;
		}
	}

	bool success = true;
	if (newHoleCount == 0)
	{
		header.holeEntryCount = 0;
		header.holeRecordOffset = 0;
		header.holeRecordSize = 0;
	}
	else
	{
		header.holeEntryCount = newHoleCount;

		uint32_t newHoleRecordSize = newHoleCount * sizeof(HoleRecord);
		header.holeRecordSize = newHoleRecordSize;

		HoleRecord* holeTable = new HoleRecord[newHoleCount];
		HoleRecord* cursor = holeTable;

		tHoleSet::iterator it = holes.begin();
		while (it != holes.end())
		{
			*(cursor++) = *(it++);
		}

		success = WriteFileSpan(holeTable, header.holeRecordOffset, newHoleRecordSize);
		delete[] holeTable;
	}

	shouldWriteHoles = false;
	return success;
}

bool cGZDBSegmentPackedFile::WriteCompressedSetRecord(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0 || !shouldWriteCompressedRecords)
	{
		return true;
	}

	cGZPersistResourceKey key(TypeID_PackedFileCompressedSet, TypeID_PackedFileCompressedSet, InstanceID_PackedFileCompressedSet);
	DoDeleteRecord(key);

	uint32_t compressedRecordCount = compressedRecords.size();
	CompressedRecordData* compressedRecInfo = new CompressedRecordData[compressedRecordCount];
	CompressedRecordData* cursor = compressedRecInfo;
	
	for (tCompressedRecordsMap::iterator it = compressedRecords.begin(); it != compressedRecords.end(); it++)
	{
		cursor->key = it->second.key;
		cursor->uncompressedSize = it->second.uncompressedSize;
		cursor++;
	}

	bool succeeded = DoWriteRecord(key, compressedRecInfo, compressedRecordCount * sizeof(CompressedRecordData));
	if (succeeded)
	{
		shouldWriteCompressedRecords = false;
	}

	delete[] compressedRecInfo;
	return succeeded;
}

bool cGZDBSegmentPackedFile::WriteRecordInternal(cGZPersistDBSerialRecord* record, cGZPersistResourceKey const& key)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
	if (infoIt != recordInfoTable->end())
	{
		IncrementTypeAndGroupUse(infoIt->second.key.type, infoIt->second.key.group, false);
		SetRecordCompressed(key, false, 0);
		FreeSpace(infoIt->second.offset, infoIt->second.size);

		recordInfoTable->erase(infoIt);
		shouldWriteRecords = true;
	}

	uint32_t size = record->GetSize();
	if (size == 0)
	{
		RecordDataInfo info(key, 0, 0);
		recordInfoTable->insert(std::pair<KeyTGI, RecordDataInfo>(key, info));
		IncrementTypeAndGroupUse(key.type, key.group, true);

		shouldWriteRecords = true;
		return true;
	}

	if (ShouldRecordBeCompressed(key, size))
	{
		uint8_t* data = new uint8_t[size + 2];
		data[size] = 0;
		data[size + 1] = 0;

		uint8_t* compressedData = NULL;
		uint32_t compressedSize = 0;

		if (record->SeekAbsolute(0)
			&& record->GetFieldVoid(data, size)
			&& ShouldRecordBeCompressed(data, size, compressedData, compressedSize))
		{
			uint32_t offset = 0;
			bool wroteCompressed;

			if (!AllocateSpace(offset, compressedSize))
			{
				wroteCompressed = false;
			}
			else if (!WriteFileSpan(compressedData, offset, compressedSize))
			{
				FreeSpace(offset, compressedSize);
				wroteCompressed = false;
			}
			else
			{
				RecordDataInfo info(key, offset, compressedSize);
				IncrementTypeAndGroupUse(key.type, key.group, true);
				SetRecordCompressed(key, true, size);

				recordInfoTable->insert(std::pair<KeyTGI, RecordDataInfo>(key, info));
				shouldWriteRecords = true;
			}

			if (compressedData != NULL)
			{
				delete[] compressedData;
			}

			if (wroteCompressed)
			{
				delete[] data;
				return true;
			}
		}

		delete[] data;
	}

	uint32_t offset = 0;
	if (AllocateSpace(offset, size))
	{
		cGZDBWriteRecordPackedFile* writeRecord = static_cast<cGZDBWriteRecordPackedFile*>(record);
		if (writeRecord->GetAccessFlags() == (GZFileAccessRead | GZFileAccessWrite)
			&& writeRecord->WriteRecordToParent(offset, size))
		{
			RecordDataInfo info(key, offset, size);
			recordInfoTable->insert(std::pair<KeyTGI, RecordDataInfo>(key, info));
			IncrementTypeAndGroupUse(key.type, key.group, true);

			shouldWriteRecords = true;
			return true;
		}

		FreeSpace(offset, size);
	}

	return false;
}

bool cGZDBSegmentPackedFile::CopyDatabaseRecord(cIGZPersistDBSegment* segment, cGZPersistResourceKey const& key, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord)
{
	cRZCriticalSectionHolder lock(criticalSection);
	bool succeeded = false;

	if (copyFromSegmentToSelf)
	{
		if (!segment->TestForRecord(key))
		{
			return false;
		}

		cIGZDBSegmentPackedFile* packedFile = NULL;
		bool retry = true;

		if (segment->QueryInterface(GZIID_cIGZDBSegmentPackedFile, reinterpret_cast<void**>(&packedFile)))
		{
			if (packedFile->GetRecordCompressed(key))
			{
				uint32_t rawSize = packedFile->GetRecordSizeRaw(key);
				uint32_t decompressedSize = segment->GetRecordSize(key);

				if (decompressedSize != -1)
				{
					uint8_t* rawBuffer = new uint8_t[rawSize];
					if (packedFile->ReadRecordRaw(key, rawBuffer, rawSize) != -1 && WriteRecordRaw(key, rawBuffer, rawSize))
					{
						SetRecordCompressed(key, true, decompressedSize);
						if (!skipDeletingSourceRecord)
						{
							segment->DeleteRecord(key);
						}

						succeeded = true;
						retry = false;
					}

					delete[] rawBuffer;
				}
			}

			packedFile->Release();
			packedFile = NULL;
		}

		if (retry)
		{
			uint32_t size = segment->GetRecordSize(key);
			if (size != -1)
			{
				uint8_t* buffer = new uint8_t[size];
				if (segment->ReadRecord(key, buffer, size) && WriteRecord(key, buffer, size))
				{
					if (!skipDeletingSourceRecord)
					{
						segment->DeleteRecord(key);
					}

					succeeded = true;
				}

				delete[] buffer;
			}
		}
	}
	else
	{
		if (!TestForRecord(key))
		{
			return false;
		}

		cIGZDBSegmentPackedFile* packedFile = NULL;
		bool retry = true;

		if (segment->QueryInterface(GZIID_cIGZDBSegmentPackedFile, reinterpret_cast<void**>(&packedFile)))
		{
			if (GetRecordCompressed(key))
			{
				uint32_t rawSize = GetRecordSizeRaw(key);
				uint32_t decompressedSize = GetDecompressedRecordLength(key);

				if (decompressedSize != -1)
				{
					uint8_t* rawBuffer = new uint8_t[rawSize];
					if (ReadRecordRaw(key, rawBuffer, rawSize) != -1 && packedFile->WriteRecordRaw(key, rawBuffer, rawSize))
					{
						packedFile->SetRecordCompressed(key, true, decompressedSize);
						if (!skipDeletingSourceRecord)
						{
							DeleteRecord(key);
						}

						succeeded = true;
						retry = false;
					}

					delete[] rawBuffer;
				}
			}

			packedFile->Release();
			packedFile = NULL;
		}

		if (retry)
		{
			uint32_t size = GetRecordSize(key);
			if (size != -1)
			{
				uint8_t* buffer = new uint8_t[size];
				if (ReadRecord(key, buffer, size) && segment->WriteRecord(key, buffer, size))
				{
					if (!skipDeletingSourceRecord)
					{
						DeleteRecord(key);
					}

					succeeded = true;
				}

				delete[] buffer;
			}
		}
	}

	return succeeded;
}

bool cGZDBSegmentPackedFile::DecompressData(uint8_t* data, uint32_t size, uint8_t*& decompressedData, uint32_t& decompressedSize)
{
	cRZFastCompression3 compressor;
	if (!compressor.ValidateCompressedData(data, size))
	{
		return false;
	}

	uint32_t realDecompressedSize = compressor.GetLengthOfCompressedData(data);
	uint8_t* originalDecompressedDataPtr = decompressedData;
	uint8_t* target;

	if (decompressedData == NULL)
	{
		decompressedSize = realDecompressedSize;
		target = new uint8_t[realDecompressedSize];
		decompressedData = target;
	}
	else
	{
		target = decompressedData;
	}

	if (target != NULL && realDecompressedSize <= decompressedSize)
	{
		if (size == 0 || decompressedSize == 0 || compressor.DecompressData(data, size, target, decompressedSize))
		{
			return true;
		}

		decompressedSize = 0;
		if (originalDecompressedDataPtr == NULL)
		{
			delete[] decompressedData;
			decompressedData = NULL;
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::DecompressRecord(uint32_t fileOffset, uint32_t size, uint8_t*& decompressedData, uint32_t& decompressedSize)
{
	if (fileOffset == 0)
	{
		return false;
	}

	uint8_t* allocatedBuffer;
	uint8_t* target;

	if (size <= 2048)
	{
		allocatedBuffer = NULL;
		target = smallScratchBuffer;

		if (smallScratchBuffer == NULL)
		{
			target = new uint8_t[2048];
			smallScratchBuffer = target;
		}
	}
	else
	{
		allocatedBuffer = new uint8_t[size];
		target = allocatedBuffer;
	}

	bool result = ReadFileSpan(target, fileOffset, size) && DecompressData(target, size, decompressedData, decompressedSize);
	if (allocatedBuffer != NULL)
	{
		delete[] allocatedBuffer;
	}

	return result;
}

bool cGZDBSegmentPackedFile::DecompressRecord(cGZPersistResourceKey const& key, uint8_t*& data, uint32_t& decompressedSize)
{
	tCompressedRecordsMap::iterator compressedIt = compressedRecords.find(key);
	if (compressedIt != compressedRecords.end())
	{
		tRecordInfoTable::iterator infoIt = recordInfoTable->find(key);
		if (infoIt != recordInfoTable->end())
		{
			return DecompressRecord(infoIt->second.offset, infoIt->second.size, data, decompressedSize);
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::ReadHeaderRecord(void)
{
	if (file.SeekToPosition(startingOffset) != startingOffset
		|| !file.Read(&header, sizeof(header)))
	{
		return false;
	}

	if (VerifyHeaderRecordIntegrity())
	{
		return true;
	}

	uint32_t headerRecordOffset = FindHeaderRecord();
	startingOffset = headerRecordOffset;

	static bool bHereAlready = false;
	if (headerRecordOffset == -1)
	{
		startingOffset = 0;
	}
	else if (!bHereAlready)
	{
		bHereAlready = true;
		bool result = ReadHeaderRecord();
		bHereAlready = false;

		return result;
	}

	return false;
}

uint32_t cGZDBSegmentPackedFile::FindHeaderRecord(void)
{
	bool wasFileOpen = file.IsOpen();
	if (!wasFileOpen)
	{
		file.SetPath(path);
		file.Open(GZFileAccessRead, GZFileOpenExistingOnly, GZFileShareRead);

		if (!file.IsOpen())
		{
			return -1;
		}
	}

	static const uint32_t bufferSize = 512;
	static char kFileIdentifierArray[] = {
		(char)0x7F, (char)0x9D, (char)0x88, (char)0xEC,
		(char)0x8F, (char)0x24, (char)0x03, (char)0x6C,
		(char)0xC9, (char)0xA6, (char)0x31, (char)0x56,
		(char)0x5B, (char)0xCF, (char)0x77, (char)0x22,
	};

	if (file.Length() <= sizeof(kFileIdentifierArray))
	{
		return -1;
	}

	std::string haystack(bufferSize, '\0');
	std::string needle(kFileIdentifierArray);

	uint32_t byteCount = bufferSize;
	file.SeekToBegin();

	uint32_t result = -1;
	while (byteCount == bufferSize)
	{
		uint32_t originalPosition = file.Position();
		byteCount = bufferSize;

		if (!file.ReadWithCount(&haystack[0], byteCount))
		{
			break;
		}

		haystack.resize(byteCount);
		int offset = haystack.find(needle);
		if (offset != -1)
		{
			uint32_t indexPosition = file.Position();
			result = originalPosition + offset + sizeof(kFileIdentifierArray);
			file.SeekToPosition(result);

			char signature[4];
			if (file.Read(signature, sizeof(signature))
				&& signature[0] == 'D'
				&& signature[1] == 'B'
				&& signature[2] == 'P'
				&& signature[3] == 'F')
			{
				break;
			}

			file.SeekToPosition(indexPosition);
		}

		file.SeekToRelativePosition(-sizeof(kFileIdentifierArray));
	}

	if (!wasFileOpen)
	{
		file.Close();
	}

	return result;
}

bool cGZDBSegmentPackedFile::ReadIndexRecord(void)
{
	if (header.indexEntryCount == 0)
	{
		if (recordInfoTable != NULL)
		{
			return true;
		}

		recordInfoTable = new tRecordInfoTable();
		return true;
	}

	if (file.SeekToPosition(startingOffset + header.indexRecordOffset) != startingOffset + header.indexRecordOffset)
	{
		return false;
	}

	RecordDataInfo* recordData = new RecordDataInfo[header.indexEntryCount];
	if (!file.Read(recordData, header.indexEntryCount * sizeof(RecordDataInfo)))
	{
		delete[] recordData;
		return false;
	}

	if (recordInfoTable == NULL)
	{
		recordInfoTable = new tRecordInfoTable();
	}

	RecordDataInfo* cursor = recordData;
	for (uint32_t i = 0; i < header.indexEntryCount; i++)
	{
		RecordDataInfo info(cursor->key, cursor->offset, cursor->size);
		recordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(cursor->key, info));

		IncrementTypeAndGroupUse(cursor->key.type, cursor->key.group, true);
		cursor++;
	}

	if (VerifyIndexRecordIntegrity())
	{
		shouldWriteRecords = false;
		delete[] recordData;
		return true;
	}

	delete[] recordData;
	return false;
}

bool cGZDBSegmentPackedFile::ReadHoleRecord(void)
{
	if (header.holeEntryCount == 0)
	{
		return true;
	}

	if (file.SeekToPosition(startingOffset + header.holeRecordOffset) != startingOffset + header.holeRecordOffset)
	{
		return false;
	}

	HoleRecord* holeRecords = new HoleRecord[header.holeEntryCount];
	if (!file.Read(holeRecords, header.holeEntryCount * sizeof(HoleRecord)))
	{
		delete[] holeRecords;
		return false;
	}

	HoleRecord* cursor = holeRecords;
	for (uint32_t i = 0; i < header.holeEntryCount; i++)
	{
		holes.insert(*cursor);
		cursor++;
	}
	
	bool success = VerifyHoleRecordIntegrity();
	if (success)
	{
		shouldWriteHoles = false;
	}

	delete[] holeRecords;
	return success;
}

bool cGZDBSegmentPackedFile::ReadCompressedSetRecord(void)
{
	compressedRecords.clear();

	cGZPersistResourceKey key(TypeID_PackedFileCompressedSet, TypeID_PackedFileCompressedSet, InstanceID_PackedFileCompressedSet);
	uint32_t compressedSize = 0;

	if (DoReadRecord(key, 0, compressedSize) == -1)
	{
		return true;
	}

	uint8_t* data = new uint8_t[compressedSize + 4];
	if (!DoReadRecord(key, data, compressedSize))
	{
		delete[] data;
		return false;
	}

	uint32_t recordCount = compressedSize / sizeof(CompressedRecordData);
	if (recordCount == 0)
	{
		delete[] data;
		return true;
	}

	CompressedRecordData* cursor = reinterpret_cast<CompressedRecordData*>(data);
	for (uint32_t i = 0; i < recordCount; i++)
	{
		compressedRecords.insert(std::pair<cGZPersistResourceKey, CompressedRecordData>(cursor->key, *cursor));
		cursor++;
	}

	delete[] data;
	return true;
}

bool cGZDBSegmentPackedFile::VerifyHeaderRecordIntegrity(void)
{
	return header.signature == kPackedFileHeaderFileIdentifier
		&& header.majorVersion <= kPackedFileHeaderMajorVersion
		&& header.indexEntryCount < INT_MAX
		&& header.indexRecordOffset < file.Length()
		&& header.indexRecordOffset + header.indexTableSize <= file.Length()
		&& header.holeEntryCount < INT_MAX
		&& header.holeRecordOffset < file.Length()
		&& header.holeRecordOffset + header.holeRecordSize <= file.Length();
}

bool cGZDBSegmentPackedFile::VerifyHoleRecordIntegrity(void)
{
	uint32_t offset = header.holeRecordOffset;
	if (offset != 0 && (offset < sizeof(header) || offset >= file.Length()))
	{
		return false;
	}

	return header.holeEntryCount == header.holeRecordSize / sizeof(HoleRecord);
}

bool cGZDBSegmentPackedFile::VerifyIndexRecordIntegrity(void)
{
	uint32_t fileLength = file.Length();
	for (tRecordInfoTable::iterator it = recordInfoTable->begin(); it != recordInfoTable->end(); it++)
	{
		uint32_t recordOffset = it->second.offset;
		if (recordOffset < sizeof(header)
			|| fileLength <= recordOffset
			|| fileLength < recordOffset + it->second.size)
		{
			return false;
		}
	}

	return true;
}

bool cGZDBSegmentPackedFile::ShouldCompactDatabase(void)
{
	if ((fileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	uint32_t fileSize = file.Length();
	uint32_t holeSize = 0;

	for (tHoleSet::iterator it = holes.begin(); it != holes.end(); it++)
	{
		holeSize = it->size;
	}

	return fileSize >= holeSize
		&& fileSize - holeSize != 0
		&& (float)holeSize / (float)(fileSize - holeSize) > holeCompactionMinRatio;
}

bool cGZDBSegmentPackedFile::ShouldRecordBeCompressed(cGZPersistResourceKey const& key, uint32_t size)
{
	const uint32_t kMaxSizeForCompression = 15999909;
	if (size > kMaxSizeForCompression || key.type == TypeID_PackedFileCompressedSet)
	{
		return false;
	}

	return compressAllRecords
		|| (compressedTypeIDs.size() != 0 && compressedTypeIDs.find(key.type) != compressedTypeIDs.end())
		|| (compressedGroupIDs.size() != 0 && compressedGroupIDs.find(key.group) != compressedGroupIDs.end());
}

bool cGZDBSegmentPackedFile::ShouldRecordBeCompressed(uint8_t* data, uint32_t size, uint8_t*& compressedData, uint32_t& compressedSize)
{
	const uint32_t kMaxSizeForCompression = 15999951;

	compressedData = NULL;
	compressedSize = 0;

	if (size >= kMaxSizeForCompression || CompressData(data, size, compressedData, compressedSize))
	{
		if ((float)compressedSize / (float)size < 0.8)
		{
			return true;
		}

		if (compressedData != NULL)
		{
			delete[] compressedData;
		}

		compressedData = NULL;
		compressedSize = 0;
	}

	return false;
}

bool cGZDBSegmentPackedFile::CompressData(uint8_t* data, uint32_t size, uint8_t*& compressedData, uint32_t& compressedSize)
{
	uint8_t* originalTarget = compressedData;
	if (size >= 40000000)
	{
		if (originalTarget == NULL)
		{
			compressedSize = size;
			originalTarget = new uint8_t[size];
			compressedData = originalTarget;
		}
		else
		{
			if (compressedSize < size)
			{
				compressedSize = 0;
				return false;
			}

			compressedSize = size;
			originalTarget = compressedData;
		}

		memcpy(originalTarget, data, size);
		return true;
	}

	cRZFastCompression3 compressor;
	uint8_t* compressedTarget;

	if (originalTarget == NULL)
	{
		uint32_t maxLengthNeeded = compressor.GetMaxLengthRequiredForCompressedData(size);
		compressedSize = maxLengthNeeded;
		compressedTarget = new uint8_t[maxLengthNeeded];
		compressedData = compressedTarget;
	}
	else
	{
		compressedTarget = compressedData;
	}

	if (compressedTarget != NULL)
	{
		if (size > 250000)
		{
			compressor.EnableQuickCompression(true);
			compressedTarget = compressedData;
		}

		if (compressor.CompressData(data, size, compressedTarget, compressedSize))
		{
			return true;
		}

		compressedSize = 0;
		if (originalTarget == NULL)
		{
			if (compressedData != NULL)
			{
				delete[] compressedData;
			}

			compressedData = NULL;
		}
	}

	return false;
}

void cGZDBSegmentPackedFile::IncrementTypeAndGroupUse(uint32_t type, uint32_t group, bool add)
{
	if (!typeAndGroupUseTrackingEnabled)
	{
		return;
	}

	tUseCountMap::iterator typeIt = typeUseCount.find(type);
	if (typeIt == typeUseCount.end())
	{
		if (add)
		{
			typeUseCount.insert(std::pair<uint32_t, int>(type, 1));
		}
	}
	else
	{
		int previousCount = typeIt->second;
		if (add)
		{
			typeIt->second++;
		}
		else if (previousCount <= 1)
		{
			typeUseCount.erase(typeIt);
		}
		else
		{
			typeIt->second--;
		}
	}

	tUseCountMap::iterator groupIt = groupUseCount.find(group);
	if (groupIt == groupUseCount.end())
	{
		if (add)
		{
			groupUseCount.insert(std::pair<uint32_t, int>(group, 1));
		}
	}
	else
	{
		int previousCount = groupIt->second;
		if (add)
		{
			groupIt->second++;
		}
		else if (previousCount <= 1)
		{
			groupUseCount.erase(groupIt);
		}
		else
		{
			groupIt->second--;
		}
	}
}

bool cGZDBSegmentPackedFile::CloseOpenRecords(void)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	if (openRecords.size() == 0)
	{
		return true;
	}

	std::list<cGZPersistDBSerialRecord*> recordsToClose;
	for (tOpenRecordMap::iterator it = openRecords.begin(); it != openRecords.end(); it++)
	{
		recordsToClose.push_back(it->second);
	}

	for (std::list<cGZPersistDBSerialRecord*>::iterator it = recordsToClose.begin(); it != recordsToClose.end(); it++)
	{
		CloseRecord(*it);
	}

	return true;
}