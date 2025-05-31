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
#include <hash_map>
#include <hash_set>
#include <map>
#include <set>
#include "cGZPersistDBSegment.h"
#include "cGZPersistResourceKey.h"
#include "cIGZDBSegmentPackedFile.h"
#include "cRZFile.h"

static const GZGUID GZCLSID_cGZDBSegmentPackedFile = 0x086D4A89;

class cGZPersistDBSerialRecord;

enum DBPFHoleUseFlags
{
	DBPFEnableHoleCompaction = 1,
	DBPFEnableHoleReuse = 2,
};

class cGZDBSegmentPackedFile : public cIGZDBSegmentPackedFile, public cGZPersistDBSegment
{
public:
	cGZDBSegmentPackedFile(void);
	virtual ~cGZDBSegmentPackedFile(void);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual cIGZPersistDBSegment* AsIGZPersistDBSegment(void);

	virtual bool Init(void);
	virtual bool Shutdown(void);

	virtual uint32_t GetFileStartingOffset(void) const;
	virtual bool SetFileStartingOffset(uint32_t offset);

	virtual void GetPath(cIGZString& path) const;
	virtual bool SetPath(cIGZString const& path);

	virtual void GetCurrentUserVersion(uint32_t& major, uint32_t& minor);
	virtual void GetNextUserVersion(uint32_t& major, uint32_t& minor);
	virtual void SetNextUserVersion(uint32_t major, uint32_t minor);

	virtual bool SetFileBuffering(uint32_t readBufferSize, uint32_t writeBufferSize);
	virtual bool SetHoleUseFlags(int32_t flags);
	virtual void SetHoleCompactionMinimumRatio(float ratio);

	virtual bool IsRecordOpen(cGZPersistResourceKey const& key);
	virtual bool GetRecordAsIGZFile(cIGZPersistDBRecord* record, cIGZFile** fileOut);
	virtual uint32_t GetRecordSizeRaw(cGZPersistResourceKey const& key);
	virtual uint32_t ReadRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t& bytesRead);
	virtual bool WriteRecordRaw(cGZPersistResourceKey const& key, void* data, uint32_t size);
	virtual uint32_t GetRecordPosition(cGZPersistResourceKey const& key);

	virtual uint32_t GetUsedTypeCount(void);
	virtual uint32_t GetUsedGroupCount(void);
	virtual void EnableUsedTypeAndGroupCounting(bool enabled);

	virtual uint32_t CopyDatabaseRecords(cIGZPersistDBSegment* segment, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord);
	virtual uint32_t CopyDatabaseRecords(cIGZString const& path, cIGZPersistResourceKeyFilter* filter, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord);

	virtual bool CompactDatabase(void);
	virtual bool VerifyDatabase(void);

	virtual bool EnableCompressionForAllRecords(bool flag);
	virtual bool EnableCompressionByType(uint32_t type, bool flag);
	virtual bool EnableCompressionByGroup(uint32_t group, bool flag);
	virtual bool GetRecordCompressed(cGZPersistResourceKey const& key);
	virtual void SetRecordCompressed(cGZPersistResourceKey const& key, bool flag, uint32_t size);

	virtual bool AllocateSpace(uint32_t& offset, uint32_t sizeToAlloc);
	virtual bool FreeSpace(uint32_t holeStart, uint32_t holeSize);
	virtual bool ReallocateSpace(cGZPersistResourceKey const& key, uint32_t&, uint32_t);
	virtual bool GetFreeSpaceMetrics(uint32_t& holeCount, uint32_t& totalHoleSize);

public:
	virtual void FinalRelease(void);
	virtual bool Init(uint32_t segmentID, cIGZString const* path, bool allowOpen);

	virtual bool Open(bool readable, bool writable);
	virtual bool IsOpen(void) const;

	virtual bool Close(void);
	virtual bool Flush(void);

	virtual uint32_t GetSegmentID(void) const;
	virtual bool SetSegmentID(uint32_t const& id);

	virtual bool DoOpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t flags);
	virtual bool DoCloseRecord(cIGZPersistDBRecord* record);
	virtual bool DoAbortRecord(cIGZPersistDBRecord* record);
	virtual bool DoTestForRecord(cGZPersistResourceKey const& key);
	virtual bool DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr);
	virtual bool DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr);
	virtual bool DoDeleteRecord(cGZPersistResourceKey const& key);
	virtual int32_t DoGetRecordSize(cGZPersistResourceKey const& key);
	virtual uint32_t DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length);
	virtual bool DoReadRecord(cGZPersistResourceKey const& key, cIGZString const& filename);
	virtual bool DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length);
	virtual bool DoWriteRecord(cGZPersistResourceKey const& key, cIGZString const& filename);

	virtual bool GetModificationTime(uint32_t& creationTime, uint32_t& modificationTime);
	virtual bool SeekAbsolute(int32_t position);
	virtual uint32_t ReadFileSpan(void* data, uint32_t offset, uint32_t length);
	virtual bool ReadFileSpanWithCount(void* data, uint32_t offset, uint32_t& length);
	virtual bool WriteFileSpan(void const* data, uint32_t offset, uint32_t length);

	virtual uint32_t GetRecordCount(cIGZPersistResourceKeyFilter* filter);
	virtual uint32_t GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter);

protected:
	bool IsOpenInternal(void) const { return (fileAccessFlags & GZFileAccessReadWrite) != 0; }
	uint32_t GetDecompressedRecordLength(cGZPersistResourceKey const& key);

	bool WriteEmptyHeaderRecord(void);
	bool WriteHeaderRecord(void);
	bool WriteIndexRecord(void);
	bool WriteHoleRecord(void);
	bool WriteCompressedSetRecord(void);
	bool WriteRecordInternal(cGZPersistDBSerialRecord* record, cGZPersistResourceKey const& key);
	void DoPostRecordClose(void);

	bool CopyDatabaseRecord(cIGZPersistDBSegment* segment, cGZPersistResourceKey const& key, bool copyFromSegmentToSelf, bool skipDeletingSourceRecord);

	bool DecompressData(uint8_t* data, uint32_t size, uint8_t*& decompressedData, uint32_t& decompressedSize);
	bool DecompressRecord(uint32_t fileOffset, uint32_t size, uint8_t*& decompressedData, uint32_t& decompressedSize);
	bool DecompressRecord(cGZPersistResourceKey const& key, uint8_t*& data, uint32_t& decompressedSize);

	bool ReadHeaderRecord(void);
	uint32_t FindHeaderRecord(void);
	bool ReadIndexRecord(void);
	bool ReadHoleRecord(void);
	bool ReadCompressedSetRecord(void);

	bool VerifyHeaderRecordIntegrity(void);
	bool VerifyHoleRecordIntegrity(void);
	bool VerifyIndexRecordIntegrity(void);

	bool ShouldCompactDatabase(void);
	bool ShouldRecordBeCompressed(cGZPersistResourceKey const& key, uint32_t size);
	bool ShouldRecordBeCompressed(uint8_t* data, uint32_t size, uint8_t*& compressedData, uint32_t& compressedSize);
	bool CompressData(uint8_t* data, uint32_t size, uint8_t*& compressedData, uint32_t& compressedSize);

	void IncrementTypeAndGroupUse(uint32_t type, uint32_t group, bool add);
	bool CloseOpenRecords(void);

protected:
	struct KeyTGI
	{
	public:
		KeyTGI(cGZPersistResourceKey const& key) :
			type(key.type),
			group(key.group),
			instance(key.instance)
		{
		}

		KeyTGI& operator=(const KeyTGI& other)
		{
			type = other.type;
			group = other.group;
			instance = other.instance;

			return *this;
		}

		operator cGZPersistResourceKey() const
		{
			return cGZPersistResourceKey(type, group, instance);
		}

		bool operator==(KeyTGI const& other)
		{
			return type == other.type && group == other.group && instance == other.instance;
		}

		friend bool operator==(KeyTGI const& left, KeyTGI const& right)
		{
			return left.type == right.type && left.group == right.group && left.instance == right.instance;
		}

	public:
		uint32_t type;
		uint32_t group;
		uint32_t instance;
	};

	struct RecordDataInfo
	{
	public:
		RecordDataInfo() { }
		RecordDataInfo(cGZPersistResourceKey const& key, uint32_t offset, uint32_t size)
		{
			this->key = key;
			this->offset = offset;
			this->size = size;
		}

		operator cGZPersistResourceKey() const
		{
			return cGZPersistResourceKey(key.type, key.group, key.instance);
		}

	public:
		cGZPersistResourceKey key;
		uint32_t offset;
		uint32_t size;
	};

	struct CompressedRecordData
	{
	public:
		CompressedRecordData() { }
		CompressedRecordData(cGZPersistResourceKey const& key, uint32_t size)
		{
			this->key = key;
			uncompressedSize = size;
		}

	public:
		cGZPersistResourceKey key;
		uint32_t uncompressedSize;
	};

	struct HoleRecord
	{
	public:
		HoleRecord() { }
		HoleRecord(uint32_t offset, uint32_t size)
		{
			this->offset = offset;
			this->size = size;
		}

		bool operator== (HoleRecord const& other)
		{
			return offset == other.offset && size == other.size;
		}

		bool operator< (HoleRecord const& other)
		{
			return offset < other.offset;
		}

		bool operator> (HoleRecord const& other)
		{
			return !(*this < other);
		}

		friend bool operator==(HoleRecord const& left, HoleRecord const& right)
		{
			return left.offset == right.offset && left.size == right.size;
		}

		friend bool operator< (HoleRecord const& left, HoleRecord const& right)
		{
			return left.offset < right.offset;
		}

		friend bool operator> (HoleRecord const& left, HoleRecord const& right)
		{
			return !(left < right);
		}

	public:
		uint32_t offset;
		uint32_t size;
	};

	struct KeyTGIHash
	{
	public:
		size_t operator()(KeyTGI const key) const { return key.instance; }
	};

	struct ResourceKeyHash
	{
	public:
		size_t operator()(cGZPersistResourceKey const key) const;
	};

protected:
	typedef std::hash_map<KeyTGI, RecordDataInfo, KeyTGIHash> tRecordInfoTable;
	typedef std::map<cGZPersistResourceKey, cGZPersistDBSerialRecord*> tOpenRecordMap;
	typedef std::hash_map<uint32_t, int> tUseCountMap;
	typedef std::hash_map<cGZPersistResourceKey, CompressedRecordData, ResourceKeyHash> tCompressedRecordsMap;
	typedef std::set<HoleRecord> tHoleSet;

protected:
	bool inited;
	uint32_t segmentID;
	uint32_t fileAccessFlags;

	uint32_t unknown;
	tRecordInfoTable* recordInfoTable;
	bool shouldWriteRecords;

	cRZString path;
	cRZFile file;

	uint32_t startingOffset;
	uint32_t readBufferSize;
	uint32_t writeBufferSize;

	uint32_t nextUserVersionMajor;
	uint32_t nextUserVersionMinor;

	uint32_t sizeLimitForInMemoryRecords;
	tOpenRecordMap openRecords;

	////////////////////////////////////////
	// BEGIN DBPF HEADER
	////////////////////////////////////////

	static const uint32_t kPackedFileHeaderFileIdentifier = 0x46504244;
	static const uint32_t kPackedFileHeaderMajorVersion = 1;
	static const uint32_t kPackedFileHeaderMinorVersion = 0;

	struct DBPFHeader {
		uint32_t signature;
		uint32_t majorVersion;
		uint32_t minorVersion;
		uint32_t currentUserVersionMajor;
		uint32_t currentUserVersionMinor;

		uint32_t __reserved0;

		uint32_t timestampCreated;
		uint32_t timestampModified;

		uint32_t indexMajorVersion;
		uint32_t indexEntryCount;
		uint32_t indexRecordOffset;
		uint32_t indexTableSize;

		uint32_t holeEntryCount;
		uint32_t holeRecordOffset;
		uint32_t holeRecordSize;

		uint32_t indexMinorVersion;
		uint32_t indexTableOffset;

		char __reserved1[28];
	} header;

	////////////////////////////////////////
	// END DBPF HEADER
	////////////////////////////////////////

	uint8_t holeUseFlags;
	float holeCompactionMinRatio;
	tHoleSet holes;
	bool shouldWriteHoles;
	bool compacted;

	bool typeAndGroupUseTrackingEnabled;
	tUseCountMap typeUseCount;
	tUseCountMap groupUseCount;

	bool shouldWriteCompressedRecords;
	bool compressAllRecords;
	std::hash_set<uint32_t> compressedTypeIDs;
	std::hash_set<uint32_t> compressedGroupIDs;
	tCompressedRecordsMap compressedRecords;

	uint8_t* smallScratchBuffer;
};