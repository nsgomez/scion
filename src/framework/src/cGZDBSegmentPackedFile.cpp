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
	if (!mInited)
	{
		mInited = true;
	}

	return true;
}

bool cGZDBSegmentPackedFile::Shutdown(void)
{
	if (mInited)
	{
		mInited = false;
		if (IsOpenInternal())
		{
			Close();
		}

		delete mSmallScratchBuffer;
		mSmallScratchBuffer = NULL;
	}

	return true;
}

uint32_t cGZDBSegmentPackedFile::GetFileStartingOffset(void) const
{
	return mStartingOffset;
}

bool cGZDBSegmentPackedFile::SetFileStartingOffset(uint32_t offset)
{
	if (IsOpenInternal())
	{
		return false;
	}

	mStartingOffset = offset;
	return true;
}

void cGZDBSegmentPackedFile::GetPath(cIGZString& path) const
{
	path.FromChar(mPath.Data(), mPath.Strlen());
}

bool cGZDBSegmentPackedFile::SetPath(cIGZString const& path)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (!IsOpenInternal())
	{
		mPath = path;
		return true;
	}

	return false;
}

void cGZDBSegmentPackedFile::GetCurrentUserVersion(uint32_t& major, uint32_t& minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	major = mHeader.currentUserVersionMajor;
	minor = mHeader.currentUserVersionMinor;
}

void cGZDBSegmentPackedFile::GetNextUserVersion(uint32_t& major, uint32_t& minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	major = mNextUserVersionMajor;
	minor = mNextUserVersionMinor;
}

void cGZDBSegmentPackedFile::SetNextUserVersion(uint32_t major, uint32_t minor)
{
	cRZCriticalSectionHolder lock(criticalSection);
	mNextUserVersionMajor = major;
	mNextUserVersionMinor = minor;
}

bool cGZDBSegmentPackedFile::SetFileBuffering(uint32_t readBufferSize, uint32_t writeBufferSize)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (mFile.SetBuffering(readBufferSize, writeBufferSize))
	{
		mReadBufferSize = readBufferSize;
		mWriteBufferSize = writeBufferSize;
		return true;
	}

	return false;
}

bool cGZDBSegmentPackedFile::SetHoleUseFlags(int32_t flags)
{
	cRZCriticalSectionHolder lock(criticalSection);
	mHoleUseFlags = flags;
	return true;
}

void cGZDBSegmentPackedFile::SetHoleCompactionMinimumRatio(float ratio)
{
	mHoleCompactionMinRatio = ratio;
}

bool cGZDBSegmentPackedFile::IsRecordOpen(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return mOpenRecords.find(key) != mOpenRecords.end();
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
		tRecordInfoTable::iterator it = mRecordInfoTable->find(key);
		if (it != mRecordInfoTable->end())
		{
			size = it->second.size;
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

	if (mFileAccessFlags & GZFileAccessWrite)
	{
		tOpenRecordMap::iterator it = mOpenRecords.find(key);
		if (it != mOpenRecords.end() && (it->second->GetAccessFlags() & GZFileAccessWrite) != 0)
		{
			return -1;
		}
	}

	tRecordInfoTable::iterator it = mRecordInfoTable->find(key);
	if (it == mRecordInfoTable->end())
	{
		return -1;
	}
	else if (data == NULL)
	{
		bytesRead = 0;
		tCompressedRecordsMap::iterator compressedRec = mCompressedRecords.find(key);
		return (compressedRec == mCompressedRecords.end())
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
		if (mFile.SeekToPosition(mStartingOffset + it->second.offset) == mStartingOffset + it->second.offset
			&& mFile.Read(data, bytesToRead))
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
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator serialIt = mOpenRecords.find(key);
	if (serialIt != mOpenRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt != mRecordInfoTable->end())
	{
		IncrementTypeAndGroupUse(key.type, key.group, false);
		SetRecordCompressed(key, false, 0);
		mRecordInfoTable->erase(infoIt);
		FreeSpace(infoIt->second.offset, infoIt->second.size);
		mShouldWriteRecords = true;
	}

	uint32_t offset = 0;
	if (AllocateSpace(offset, size))
	{
		RecordDataInfo newRecord(offset, size);
		if (WriteFileSpan(data, offset, size))
		{
			mRecordInfoTable->insert(std::pair<const cGZPersistResourceKey, RecordDataInfo>(key, newRecord));
			IncrementTypeAndGroupUse(key.type, key.group, true);
			mShouldWriteRecords = true;
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
	uint32_t position = -1;

	if (IsOpenInternal())
	{
		tRecordInfoTable::iterator it = mRecordInfoTable->find(key);
		if (it != mRecordInfoTable->end())
		{
			position = it->second.offset;
		}
	}

	return position;
}

uint32_t cGZDBSegmentPackedFile::GetUsedTypeCount(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count;

	if (mTypeUseCount.size() == 0)
	{
		std::set<uint32_t> set;
		for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
		{
			set.insert(it->first.type);
		}

		for (tOpenRecordMap::iterator it = mOpenRecords.begin(); it != mOpenRecords.end(); it++)
		{
			set.insert(it->first.type);
		}

		count = set.size();
	}
	else
	{
		count = mTypeUseCount.size();
	}

	return count;
}

uint32_t cGZDBSegmentPackedFile::GetUsedGroupCount(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count;

	if (mGroupUseCount.size() == 0)
	{
		std::set<uint32_t> set;
		for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
		{
			set.insert(it->first.group);
		}

		for (tOpenRecordMap::iterator it = mOpenRecords.begin(); it != mOpenRecords.end(); it++)
		{
			set.insert(it->first.group);
		}

		count = set.size();
	}
	else
	{
		count = mGroupUseCount.size();
	}

	return count;
}

void cGZDBSegmentPackedFile::EnableUsedTypeAndGroupCounting(bool enabled)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (mTypeAndGroupUseTrackingEnabled != enabled)
	{
		mTypeAndGroupUseTrackingEnabled = enabled;

		mTypeUseCount.clear();
		mGroupUseCount.clear();

		if (mTypeAndGroupUseTrackingEnabled)
		{
			for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
			{
				IncrementTypeAndGroupUse(it->first.type, it->first.group, true);
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
	mCompressAllRecords = flag;
	return true;
}

bool cGZDBSegmentPackedFile::EnableCompressionByType(uint32_t type, bool flag)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (flag)
	{
		mCompressedTypeIDs.insert(type);
	}
	else
	{
		mCompressedTypeIDs.erase(type);
	}

	return false;
}

bool cGZDBSegmentPackedFile::EnableCompressionByGroup(uint32_t group, bool flag)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (flag)
	{
		mCompressedGroupIDs.insert(group);
	}
	else
	{
		mCompressedGroupIDs.erase(group);
	}

	return false;
}

bool cGZDBSegmentPackedFile::GetRecordCompressed(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return mCompressedRecords.find(key) != mCompressedRecords.end();
}

void cGZDBSegmentPackedFile::SetRecordCompressed(cGZPersistResourceKey const& key, bool flag, uint32_t size)
{
	cRZCriticalSectionHolder lock(criticalSection);
	tCompressedRecordsMap::iterator it = mCompressedRecords.find(key);
	if (flag)
	{
		if (it == mCompressedRecords.end())
		{
			CompressedRecordData record(key, size);
			mCompressedRecords.insert(std::pair<cGZPersistResourceKey, CompressedRecordData>(key, record));
			mShouldWriteCompressedRecords = true;
		}
		else
		{
			it->second.uncompressedSize = size;
		}
	}
	else if (it != mCompressedRecords.end())
	{
		mCompressedRecords.erase(it);
		mShouldWriteCompressedRecords = true;
	}
}

uint32_t cGZDBSegmentPackedFile::GetCompressedRecordLength(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t length = -1u;

	if (mCompressedRecords.find(key) != mCompressedRecords.end())
	{
		length = GetRecordSizeRaw(key);
	}

	return length;
}

bool cGZDBSegmentPackedFile::AllocateSpace(uint32_t& offset, uint32_t sizeToAlloc)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	uint32_t origFileSize = mFile.Length();
	if (mHoleUseFlags & DBPFEnableHoleReuse)
	{
		for (tHoleSet::iterator it = mHoles.begin(); it != mHoles.end(); it++)
		{
			if (sizeToAlloc <= it->size)
			{
				uint32_t holeOffset = it->offset;
				uint32_t holeSize = it->size;

				offset = holeOffset;
				mHoles.erase(it);

				FreeSpace(sizeToAlloc + holeOffset, holeSize - sizeToAlloc);
				return true;
			}
		}
	}

	if (mFile.SetLength(origFileSize + sizeToAlloc))
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
		mHoles.insert(HoleRecord(holeStart, holeSize));
		mShouldWriteHoles = true;
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

	for (tHoleSet::iterator it = mHoles.begin(); it != mHoles.end(); it++)
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

	if (mPath.Strlen() == 0)
	{
		return false;
	}

	if (!mInited)
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

	mFileAccessFlags = accessFlags;
	mFile.SetPath(mPath);

	if (!mFile.Open(mFileAccessFlags, (writable ? GZFileCreateNewOpenExisting : GZFileOpenExistingOnly), GZFileShareRead))
	{
		mFileAccessFlags = 0;
		return false;
	}

	if (mFile.Length() != 0 || ((mFileAccessFlags & GZFileAccessWrite) != 0 && WriteEmptyHeaderRecord()))
	{
		if (!ReadHeaderRecord())
		{
			if ((mFileAccessFlags & GZFileAccessWrite) != 0 && WriteEmptyHeaderRecord())
			{
				if (mRecordInfoTable == NULL)
				{
					mRecordInfoTable = new tRecordInfoTable();
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

	mFile.Close();
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
	if (mFileAccessFlags & GZFileAccessWrite)
	{
		succeeded = CloseOpenRecords() && Flush();
	}

	delete mRecordInfoTable;
	mRecordInfoTable = NULL;

	mTypeUseCount.clear();
	mGroupUseCount.clear();
	mHoles.clear();
	mCompressedRecords.clear();

	mFile.Close();
	mFileAccessFlags = 0;

	return succeeded;
}

bool cGZDBSegmentPackedFile::Flush(void)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!WriteCompressedSetRecord())
	{
		return false;
	}

	if ((mHoleUseFlags & DBPFEnableHoleCompaction) != 0 && ShouldCompactDatabase())
	{
		CompactDatabase();
	}

	return WriteIndexRecord()
		&& WriteHoleRecord()
		&& WriteHeaderRecord()
		&& mFile.Flush();
}

uint32_t cGZDBSegmentPackedFile::GetSegmentID(void) const
{
	return mSegmentID;
}

bool cGZDBSegmentPackedFile::SetSegmentID(uint32_t const& id)
{
	mSegmentID = id;
	return true;
}

uint32_t cGZDBSegmentPackedFile::GetRecordCount(cIGZPersistResourceKeyFilter* filter)
{
	cRZCriticalSectionHolder lock(criticalSection);
	if (filter == NULL)
	{
		if (mRecordInfoTable == NULL)
		{
			return 0;
		}
		else
		{
			return mRecordInfoTable->size();
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
		for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
		{
			cGZPersistResourceKey key = it->first;
			if (it->first.type != TypeID_PackedFileCompressedSet)
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
	if (!mInited)
	{
		mInited = true;
		if (path != NULL)
		{
			mPath = *path;
		}

		mSegmentID = segmentID;
	}

	return true;
}

void cGZDBSegmentPackedFile::FinalRelease(void)
{
	if (IsOpenInternal())
	{
		Close();
	}

	if (mInited)
	{
		Shutdown();
	}

	cGZPersistDBSegment::FinalRelease();
}

bool cGZDBSegmentPackedFile::DoTestForRecord(cGZPersistResourceKey const& key)
{
	return IsOpenInternal()
		&& (mRecordInfoTable->find(key) != mRecordInfoTable->end() || mOpenRecords.find(key) != mOpenRecords.end());
}

bool cGZDBSegmentPackedFile::DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator openRecordIt = mOpenRecords.find(key);
	if (openRecordIt != mOpenRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator recInfo = mRecordInfoTable->find(key);
	if (recInfo == mRecordInfoTable->end())
	{
		return DoCreateNewRecord(key, outPtr);
	}

	//cGZDBWriteRecordPackedFile* record;

	// TODO
	if (mCompressedRecords.find(key) == mCompressedRecords.end())
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

	if (mFileAccessFlags & GZFileAccessWrite)
	{
		tOpenRecordMap::iterator it = mOpenRecords.find(key);
		if (it != mOpenRecords.end())
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
		tCompressedRecordsMap::iterator it = mCompressedRecords.find(key);
		if (it != mCompressedRecords.end())
		{
			return it->second.uncompressedSize;
		}
	}
	else
	{
		tRecordInfoTable::iterator it = mRecordInfoTable->find(key);
		if (it != mRecordInfoTable->end())
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

	tOpenRecordMap::iterator openRecIt = mOpenRecords.find(key);
	cGZPersistDBRecord* record;

	if (openRecIt == mOpenRecords.end())
	{
		tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
		if (infoIt == mRecordInfoTable->end())
		{
			*outPtr = NULL;
			return false;
		}

		tCompressedRecordsMap::iterator compressedIt = mCompressedRecords.find(key);
		cGZPersistDBSerialRecord* serialRecord;

		if (compressedIt == mCompressedRecords.end())
		{
			if (infoIt->second.size <= mSizeLimitForInMemoryRecords)
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
			record = new cGZDBReadRecordRAM(original->mData, original->mSize, key, this, true);
		}
		else if (recordType == DBReadRecordTypePackedFile)
		{
			cGZDBReadRecordPackedFile* original = static_cast<cGZDBReadRecordPackedFile*>(readRecord);
			record = new cGZDBReadRecordPackedFile(original->mRecordOffset, original->mSize, key, this, true);
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
		mOpenRecords.insert(std::pair<cGZPersistResourceKey, cGZPersistDBSerialRecord*>(key, serialRecord));
	}

	*outPtr = record;
	return record != NULL;
}

bool cGZDBSegmentPackedFile::DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt != mRecordInfoTable->end())
	{
		return false;
	}

	tOpenRecordMap::iterator openIt = mOpenRecords.find(key);
	if (openIt != mOpenRecords.end())
	{
		return false;
	}

	cGZDBWriteRecordPackedFile* newRecord = new cGZDBWriteRecordPackedFile(0U, 0U, key, this, true);
	newRecord->AddRef();

	mOpenRecords.insert(std::pair<cGZPersistResourceKey, cGZPersistDBSerialRecord*>(key, newRecord));
	return true;
}

bool cGZDBSegmentPackedFile::DoCloseRecord(cIGZPersistDBRecord* record)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	cGZPersistResourceKey key = record->GetKey();
	for (tOpenRecordMap::iterator it = mOpenRecords.find(key); it != mOpenRecords.end() && it->first == key; it++)
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

			mOpenRecords.erase(it);
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
	for (tOpenRecordMap::iterator it = mOpenRecords.find(key); it != mOpenRecords.end() && it->first == key; it++)
	{
		cGZPersistDBSerialRecord* serialRecord = it->second;
		if (static_cast<cGZPersistDBRecord*>(serialRecord) == record)
		{
			static_cast<cGZDBRecord*>(record)->DoPostClose();
			serialRecord->Release();

			mOpenRecords.erase(it);
			return true;
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::DoDeleteRecord(cGZPersistResourceKey const& key)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tOpenRecordMap::iterator openIt = mOpenRecords.find(key);
	if (openIt == mOpenRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt == mRecordInfoTable->end())
	{
		return false;
	}

	IncrementTypeAndGroupUse(key.type, key.group, false);

	SetRecordCompressed(key, false, 0);
	mRecordInfoTable->erase(infoIt);
	FreeSpace(infoIt->second.offset, infoIt->second.size);

	mShouldWriteRecords = true;
	return true;
}

uint32_t cGZDBSegmentPackedFile::DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
{
	if ((mFileAccessFlags & GZFileAccessRead) != 0)
	{
		length = 0;
		return -1;
	}

	tOpenRecordMap::iterator openRecIt;
	if ((mFileAccessFlags & GZFileAccessWrite) != 0
		&& (openRecIt = mOpenRecords.find(key)) != mOpenRecords.end()
		&& (openRecIt->second->GetAccessFlags() & GZFileAccessWrite) != 0)
	{
		length = 0;
		return -1;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt == mRecordInfoTable->end())
	{
		length = 0;
		return -1;
	}

	tCompressedRecordsMap::iterator compressedIt = mCompressedRecords.find(key);
	if (data != NULL)
	{
		if (compressedIt == mCompressedRecords.end())
		{
			uint32_t bytesToRead = length;
			if (infoIt->second.size < bytesToRead)
			{
				bytesToRead = infoIt->second.size;
			}

			if (mFile.SeekToPosition(mStartingOffset + infoIt->second.offset) != mStartingOffset + infoIt->second.offset
				|| !mFile.Read(data, bytesToRead))
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
		if (compressedIt == mCompressedRecords.end())
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
	if ((mFileAccessFlags & GZFileAccessWrite) == 0
		|| (openRecIt = mOpenRecords.find(key)) != mOpenRecords.end())
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt != mRecordInfoTable->end())
	{
		uint32_t offset = infoIt->second.offset;
		uint32_t size = infoIt->second.size;

		IncrementTypeAndGroupUse(key.type, key.group, false);
		SetRecordCompressed(key, false, 0);

		mRecordInfoTable->erase(infoIt);
		FreeSpace(offset, size);
		mShouldWriteRecords = true;
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
		RecordDataInfo record(offset, length);
		if (WriteFileSpan(data, offset, length))
		{
			mRecordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(key, record));
			IncrementTypeAndGroupUse(key.type, key.group, true);
			mShouldWriteRecords = true;
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
		creationTime = mHeader.timestampCreated;
		modificationTime = mHeader.timestampModified;
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

	return mFile.SeekToPosition(mStartingOffset + position) == mStartingOffset + position;
}

bool cGZDBSegmentPackedFile::ReadFileSpan(void* data, uint32_t offset, uint32_t length)
{
	cRZCriticalSectionHolder lock(criticalSection);

	return IsOpenInternal()
		&& mFile.SeekToPosition(mStartingOffset + offset) == mStartingOffset + offset
		&& mFile.Read(data, length);
}

bool cGZDBSegmentPackedFile::ReadFileSpanWithCount(void* data, uint32_t offset, uint32_t& length)
{
	cRZCriticalSectionHolder lock(criticalSection);

	return IsOpenInternal()
		&& mFile.SeekToPosition(mStartingOffset + offset) == mStartingOffset + offset
		&& mFile.ReadWithCount(data, length);
}

bool cGZDBSegmentPackedFile::WriteFileSpan(void const* data, uint32_t offset, uint32_t length)
{
	cRZCriticalSectionHolder lock(criticalSection);

	return IsOpenInternal()
		&& (mFileAccessFlags & GZFileAccessWrite) != 0
		&& mFile.SeekToPosition(mStartingOffset + offset) == mStartingOffset + offset
		&& mFile.Write(data, length);
}

uint32_t cGZDBSegmentPackedFile::GetDecompressedRecordLength(cGZPersistResourceKey const& key)
{
	tCompressedRecordsMap::iterator it = mCompressedRecords.find(key);
	if (it != mCompressedRecords.end())
	{
		return it->second.uncompressedSize;
	}

	return -1;
}

bool cGZDBSegmentPackedFile::WriteEmptyHeaderRecord(void)
{
	memset(&mHeader, 0, sizeof(mHeader));

	mHeader.signature = kPackedFileHeaderFileIdentifier;
	mHeader.majorVersion = kPackedFileHeaderMajorVersion;
	mHeader.minorVersion = kPackedFileHeaderMinorVersion;
	mHeader.currentUserVersionMajor = mNextUserVersionMajor;
	mHeader.currentUserVersionMinor = mNextUserVersionMinor;

	mHeader.__reserved0 = 0;

	mHeader.timestampCreated = time(NULL);
	mHeader.timestampModified = time(NULL);

	mHeader.indexMajorVersion = 7;
	mHeader.indexEntryCount = 0;
	mHeader.indexRecordOffset = 0;
	mHeader.indexTableSize = 0;

	mHeader.holeEntryCount = 0;
	mHeader.holeRecordOffset = 0;
	mHeader.holeRecordSize = 0;

	return WriteHeaderRecord() && mFile.SetLength(sizeof(mHeader));
}

bool cGZDBSegmentPackedFile::WriteHeaderRecord(void)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0
		|| !mFile.IsOpen()
		|| mFile.SeekToPosition(mStartingOffset) != mStartingOffset)
	{
		return false;
	}

	DBPFHeader writeHeader;
	memcpy(&writeHeader, &mHeader, sizeof(writeHeader));

	writeHeader.signature = kPackedFileHeaderFileIdentifier;
	writeHeader.majorVersion = kPackedFileHeaderMajorVersion;
	writeHeader.minorVersion = kPackedFileHeaderMinorVersion;
	writeHeader.currentUserVersionMinor = mNextUserVersionMinor;
	writeHeader.timestampModified = time(NULL);

	return mFile.Write(&writeHeader, sizeof(writeHeader));
}

bool cGZDBSegmentPackedFile::WriteIndexRecord(void)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!mShouldWriteRecords)
	{
		return true;
	}

	uint32_t originalIndexSize = mHeader.indexTableSize;
	uint32_t newIndexEntryCount = mRecordInfoTable->size();
	uint32_t newIndexSize = newIndexEntryCount * sizeof(IndexRecordData);

	if (newIndexSize == 0)
	{
		bool success = FreeSpace(mHeader.indexRecordOffset, originalIndexSize);
		mShouldWriteRecords = false;
		return success;
	}

	mHeader.indexEntryCount = newIndexEntryCount;
	mHeader.indexTableSize = newIndexSize;

	if (newIndexSize - originalIndexSize != 0)
	{
		if (!FreeSpace(mHeader.indexRecordOffset, originalIndexSize))
		{
			mShouldWriteRecords = false;
			return false;
		}

		mHeader.indexRecordOffset = 0;

		if (!AllocateSpace(mHeader.indexRecordOffset, mHeader.indexTableSize))
		{
			mHeader.indexEntryCount = 0;
			mHeader.indexRecordOffset = 0;
			mHeader.indexTableSize = 0;

			mShouldWriteRecords = false;
			return false;
		}
	}

	IndexRecordData* recordTable = new IndexRecordData[newIndexEntryCount];
	IndexRecordData* cursor = recordTable;

	for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
	{
		cursor->key = it->first;
		cursor->offset = it->second.offset;
		cursor->size = it->second.size;

		cursor++;
	}

	bool success = WriteFileSpan(recordTable, mHeader.indexRecordOffset, mHeader.indexTableSize);
	delete[] recordTable;

	if (!success && mHeader.indexRecordOffset != 0)
	{
		FreeSpace(mHeader.indexRecordOffset, mHeader.indexTableSize);

		mHeader.indexEntryCount = 0;
		mHeader.indexRecordOffset = 0;
		mHeader.indexTableSize = 0;
	}

	mShouldWriteRecords = false;
	return success;
}

bool cGZDBSegmentPackedFile::WriteHoleRecord(void)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return true;
	}

	if (!mShouldWriteHoles)
	{
		return true;
	}

	uint32_t newHoleCount = mHoles.size();
	if (newHoleCount != mHeader.holeEntryCount)
	{
		bool success = FreeSpace(mHeader.holeRecordOffset, mHeader.holeRecordSize);
		if (success && mHoles.size() != 0)
		{
			uint32_t newHoleRecordSize = mHoles.size() * sizeof(HoleRecord);
			mHeader.holeRecordSize = newHoleRecordSize;

			success = AllocateSpace(mHeader.holeRecordOffset, newHoleRecordSize);
		}

		if (!success)
		{
			mHeader.holeEntryCount = 0;
			mHeader.holeRecordOffset = 0;
			mHeader.holeRecordSize = 0;

			mShouldWriteHoles = false;
			return false;
		}
	}

	bool success = true;
	if (newHoleCount == 0)
	{
		mHeader.holeEntryCount = 0;
		mHeader.holeRecordOffset = 0;
		mHeader.holeRecordSize = 0;
	}
	else
	{
		mHeader.holeEntryCount = newHoleCount;

		uint32_t newHoleRecordSize = newHoleCount * sizeof(HoleRecord);
		mHeader.holeRecordSize = newHoleRecordSize;

		HoleRecord* holeTable = new HoleRecord[newHoleCount];
		HoleRecord* cursor = holeTable;

		tHoleSet::iterator it = mHoles.begin();
		while (it != mHoles.end())
		{
			*(cursor++) = *(it++);
		}

		success = WriteFileSpan(holeTable, mHeader.holeRecordOffset, newHoleRecordSize);
		delete[] holeTable;
	}

	mShouldWriteHoles = false;
	return success;
}

bool cGZDBSegmentPackedFile::WriteCompressedSetRecord(void)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0 || !mShouldWriteCompressedRecords)
	{
		return true;
	}

	cGZPersistResourceKey key(TypeID_PackedFileCompressedSet, TypeID_PackedFileCompressedSet, InstanceID_PackedFileCompressedSet);
	DoDeleteRecord(key);

	uint32_t compressedRecordCount = mCompressedRecords.size();
	CompressedRecordData* compressedRecInfo = new CompressedRecordData[compressedRecordCount];
	CompressedRecordData* cursor = compressedRecInfo;
	
	for (tCompressedRecordsMap::iterator it = mCompressedRecords.begin(); it != mCompressedRecords.end(); it++)
	{
		cursor->key = it->second.key;
		cursor->uncompressedSize = it->second.uncompressedSize;
		cursor++;
	}

	bool succeeded = DoWriteRecord(key, compressedRecInfo, compressedRecordCount * sizeof(CompressedRecordData));
	if (succeeded)
	{
		mShouldWriteCompressedRecords = false;
	}

	delete[] compressedRecInfo;
	return succeeded;
}

bool cGZDBSegmentPackedFile::WriteRecordInternal(cGZPersistDBSerialRecord* record, cGZPersistResourceKey const& key)
{
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
	if (infoIt != mRecordInfoTable->end())
	{
		IncrementTypeAndGroupUse(infoIt->first.type, infoIt->first.group, false);
		SetRecordCompressed(key, false, 0);
		FreeSpace(infoIt->second.offset, infoIt->second.size);

		mRecordInfoTable->erase(infoIt);
		mShouldWriteRecords = true;
	}

	uint32_t size = record->GetSize();
	if (size == 0)
	{
		RecordDataInfo info(0, 0);
		mRecordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(key, info));
		IncrementTypeAndGroupUse(key.type, key.group, true);

		mShouldWriteRecords = true;
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
				RecordDataInfo info(offset, compressedSize);
				IncrementTypeAndGroupUse(key.type, key.group, true);
				SetRecordCompressed(key, true, size);

				mRecordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(key, info));
				mShouldWriteRecords = true;
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
			RecordDataInfo info(offset, size);
			mRecordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(key, info));
			IncrementTypeAndGroupUse(key.type, key.group, true);

			mShouldWriteRecords = true;
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
		target = mSmallScratchBuffer;

		if (mSmallScratchBuffer == NULL)
		{
			target = new uint8_t[2048];
			mSmallScratchBuffer = target;
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
	tCompressedRecordsMap::iterator compressedIt = mCompressedRecords.find(key);
	if (compressedIt != mCompressedRecords.end())
	{
		tRecordInfoTable::iterator infoIt = mRecordInfoTable->find(key);
		if (infoIt != mRecordInfoTable->end())
		{
			return DecompressRecord(infoIt->second.offset, infoIt->second.size, data, decompressedSize);
		}
	}

	return false;
}

bool cGZDBSegmentPackedFile::ReadHeaderRecord(void)
{
	if (mFile.SeekToPosition(mStartingOffset) != mStartingOffset
		|| !mFile.Read(&mHeader, sizeof(mHeader)))
	{
		return false;
	}

	if (VerifyHeaderRecordIntegrity())
	{
		return true;
	}

	uint32_t headerRecordOffset = FindHeaderRecord();
	mStartingOffset = headerRecordOffset;

	static bool bHereAlready = false;
	if (headerRecordOffset == -1)
	{
		mStartingOffset = 0;
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
	bool wasFileOpen = mFile.IsOpen();
	if (!wasFileOpen)
	{
		mFile.SetPath(mPath);
		mFile.Open(GZFileAccessRead, GZFileOpenExistingOnly, GZFileShareRead);

		if (!mFile.IsOpen())
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

	if (mFile.Length() <= sizeof(kFileIdentifierArray))
	{
		return -1;
	}

	std::string haystack(bufferSize, '\0');
	std::string needle(kFileIdentifierArray);

	uint32_t byteCount = bufferSize;
	mFile.SeekToBegin();

	uint32_t result = -1;
	while (byteCount == bufferSize)
	{
		uint32_t originalPosition = mFile.Position();
		byteCount = bufferSize;

		if (!mFile.ReadWithCount(&haystack[0], byteCount))
		{
			break;
		}

		haystack.resize(byteCount);
		int offset = haystack.find(needle);
		if (offset != -1)
		{
			uint32_t indexPosition = mFile.Position();
			result = originalPosition + offset + sizeof(kFileIdentifierArray);
			mFile.SeekToPosition(result);

			char signature[4];
			if (mFile.Read(signature, sizeof(signature))
				&& signature[0] == 'D'
				&& signature[1] == 'B'
				&& signature[2] == 'P'
				&& signature[3] == 'F')
			{
				break;
			}

			mFile.SeekToPosition(indexPosition);
		}

		mFile.SeekToRelativePosition(-sizeof(kFileIdentifierArray));
	}

	if (!wasFileOpen)
	{
		mFile.Close();
	}

	return result;
}

bool cGZDBSegmentPackedFile::ReadIndexRecord(void)
{
	if (mHeader.indexEntryCount == 0)
	{
		if (mRecordInfoTable != NULL)
		{
			return true;
		}

		mRecordInfoTable = new tRecordInfoTable();
		return true;
	}

	if (mFile.SeekToPosition(mStartingOffset + mHeader.indexRecordOffset) != mStartingOffset + mHeader.indexRecordOffset)
	{
		return false;
	}

	IndexRecordData* recordData = new IndexRecordData[mHeader.indexEntryCount];
	if (!mFile.Read(recordData, mHeader.indexEntryCount * sizeof(IndexRecordData)))
	{
		delete[] recordData;
		return false;
	}

	if (mRecordInfoTable == NULL)
	{
		mRecordInfoTable = new tRecordInfoTable();
	}

	IndexRecordData* cursor = recordData;
	for (uint32_t i = 0; i < mHeader.indexEntryCount; i++)
	{
		RecordDataInfo info(cursor->offset, cursor->size);
		mRecordInfoTable->insert(std::pair<cGZPersistResourceKey, RecordDataInfo>(cursor->key, info));

		IncrementTypeAndGroupUse(cursor->key.type, cursor->key.group, true);
		cursor++;
	}

	if (VerifyIndexRecordIntegrity())
	{
		mShouldWriteRecords = false;
		delete[] recordData;
		return true;
	}

	delete[] recordData;
	return false;
}

bool cGZDBSegmentPackedFile::ReadHoleRecord(void)
{
	if (mHeader.holeEntryCount == 0)
	{
		return true;
	}

	if (mFile.SeekToPosition(mStartingOffset + mHeader.holeRecordOffset) != mStartingOffset + mHeader.holeRecordOffset)
	{
		return false;
	}

	HoleRecord* holeRecords = new HoleRecord[mHeader.holeEntryCount];
	if (!mFile.Read(holeRecords, mHeader.holeEntryCount * sizeof(HoleRecord)))
	{
		delete[] holeRecords;
		return false;
	}

	HoleRecord* cursor = holeRecords;
	for (uint32_t i = 0; i < mHeader.holeEntryCount; i++)
	{
		mHoles.insert(*cursor);
		cursor++;
	}
	
	bool success = VerifyHoleRecordIntegrity();
	if (success)
	{
		mShouldWriteHoles = false;
	}

	delete[] holeRecords;
	return success;
}

bool cGZDBSegmentPackedFile::ReadCompressedSetRecord(void)
{
	mCompressedRecords.clear();

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
		mCompressedRecords.insert(std::pair<cGZPersistResourceKey, CompressedRecordData>(cursor->key, *cursor));
		cursor++;
	}

	delete[] data;
	return true;
}

bool cGZDBSegmentPackedFile::VerifyHeaderRecordIntegrity(void)
{
	return mHeader.signature == kPackedFileHeaderFileIdentifier
		&& mHeader.majorVersion <= kPackedFileHeaderMajorVersion
		&& mHeader.indexEntryCount < INT_MAX
		&& mHeader.indexRecordOffset < mFile.Length()
		&& mHeader.indexRecordOffset + mHeader.indexTableSize <= mFile.Length()
		&& mHeader.holeEntryCount < INT_MAX
		&& mHeader.holeRecordOffset < mFile.Length()
		&& mHeader.holeRecordOffset + mHeader.holeRecordSize <= mFile.Length();
}

bool cGZDBSegmentPackedFile::VerifyHoleRecordIntegrity(void)
{
	uint32_t offset = mHeader.holeRecordOffset;
	if (offset != 0 && (offset < sizeof(mHeader) || offset >= mFile.Length()))
	{
		return false;
	}

	return mHeader.holeEntryCount == mHeader.holeRecordSize / sizeof(HoleRecord);
}

bool cGZDBSegmentPackedFile::VerifyIndexRecordIntegrity(void)
{
	uint32_t fileLength = mFile.Length();
	for (tRecordInfoTable::iterator it = mRecordInfoTable->begin(); it != mRecordInfoTable->end(); it++)
	{
		uint32_t recordOffset = it->second.offset;
		if (recordOffset < sizeof(mHeader)
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
	if ((mFileAccessFlags & GZFileAccessWrite) == 0)
	{
		return false;
	}

	uint32_t fileSize = mFile.Length();
	uint32_t holeSize = 0;

	for (tHoleSet::iterator it = mHoles.begin(); it != mHoles.end(); it++)
	{
		holeSize = it->size;
	}

	return fileSize >= holeSize
		&& fileSize - holeSize != 0
		&& (float)holeSize / (float)(fileSize - holeSize) > mHoleCompactionMinRatio;
}

bool cGZDBSegmentPackedFile::ShouldRecordBeCompressed(cGZPersistResourceKey const& key, uint32_t size)
{
	const uint32_t kMaxSizeForCompression = 15999909;
	if (size > kMaxSizeForCompression || key.type == TypeID_PackedFileCompressedSet)
	{
		return false;
	}

	return mCompressAllRecords
		|| (mCompressedTypeIDs.size() != 0 && mCompressedTypeIDs.find(key.type) != mCompressedTypeIDs.end())
		|| (mCompressedGroupIDs.size() != 0 && mCompressedGroupIDs.find(key.group) != mCompressedGroupIDs.end());
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
	if (mTypeAndGroupUseTrackingEnabled)
	{
		tUseCountMap::iterator typeIt = mTypeUseCount.find(type);
		if (typeIt != mTypeUseCount.end())
		{
			if (add)
			{
				typeIt->second++;
			}
			else
			{
				int previousCount = typeIt->second;
				if (previousCount > 1)
				{
					typeIt->second = previousCount - 1;
				}
				else
				{
					mTypeUseCount.erase(typeIt);
				}
			}
		}
	}
	else if (add)
	{
		mTypeUseCount.insert(std::pair<uint32_t, int>(type, 1));
	}

	if (mTypeAndGroupUseTrackingEnabled)
	{
		tUseCountMap::iterator groupIt = mGroupUseCount.find(group);
		if (groupIt != mGroupUseCount.end())
		{
			if (add)
			{
				groupIt->second++;
			}
			else
			{
				int previousCount = groupIt->second;
				if (previousCount > 1)
				{
					groupIt->second = previousCount - 1;
				}
				else
				{
					mGroupUseCount.erase(groupIt);
				}
			}
		}
		else if (add)
		{
			mGroupUseCount.insert(std::pair<uint32_t, int>(group, 1));
		}
	}
}

bool cGZDBSegmentPackedFile::CloseOpenRecords(void)
{
	if (!IsOpenInternal())
	{
		return false;
	}

	if (mOpenRecords.size() == 0)
	{
		return true;
	}

	std::list<cGZPersistDBSerialRecord*> recordsToClose;
	for (tOpenRecordMap::iterator it = mOpenRecords.begin(); it != mOpenRecords.end(); it++)
	{
		recordsToClose.push_back(it->second);
	}

	for (std::list<cGZPersistDBSerialRecord*>::iterator it = recordsToClose.begin(); it != recordsToClose.end(); it++)
	{
		CloseRecord(*it);
	}

	return true;
}