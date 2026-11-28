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
#include <list>
#include <map>
#include "cGZDBSDFRecord.h"
#include "cGZPersistDBSegment.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistDBSegmentDirectoryFiles.h"
#include "cGZRESIDtoExtensionMap.h"
#include "cRZAutoRefCount.h"
#include "cRZCallbackTimer.h"
#include "cRZDirectory.h"
#include "cRZRandom.h"
#include "cRZString.h"

static const GZGUID GZCLSID_cGZDBSegmentDirectoryFiles = 0xA079CE1B;

class cGZDBSegmentDirectoryFiles : public cIGZPersistDBSegmentDirectoryFiles, public cGZPersistDBSegment
{
public:
	cGZDBSegmentDirectoryFiles();
	virtual ~cGZDBSegmentDirectoryFiles();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual cIGZPersistDBSegment* AsIDBSegment();

	virtual bool Init();
	virtual bool Shutdown();

	virtual cIGZString const& Path() const;
	virtual void GetPath(cIGZString& outPath) const;
	virtual bool SetPath(cIGZString const& path);

	virtual void SetDefaultGroupId(uint32_t groupId);
	virtual uint32_t GroupId() const;

	virtual cIGZPersistRESIDtoExtensionMap& ExtensionMap();
	virtual bool GetRESIDtoExtensionMap(cIGZPersistRESIDtoExtensionMap** out);
	virtual bool AddExtensionAssociation(uint32_t resId, cIGZString const& extension);

	virtual void SetAllowedIDPositions(bool allowIdAtStart, bool allowIdAtEnd);
	virtual void SetAllowTextAsKey(bool allowTextAsKey);

	virtual bool GetFileNameFromKey(cGZPersistResourceKey const& key, cIGZString& fileName) const;
	virtual bool GetFilePathFromKey(cGZPersistResourceKey const& key, cIGZString& filePath) const;
	virtual bool GetKeyFromFileName(cIGZString const& fileName, cIGZString const& extension, cGZPersistResourceKey& key) const;
	virtual bool IsFileNameValid(cIGZString const& fileName) const;

	virtual bool GenerateNewKey(uint32_t type, cGZPersistResourceKey& key);
	virtual uint32_t EnumerateRecordFiles(cIGZString** names, uint32_t maxCount);

	virtual bool GetFileChangeDetection() const;
	virtual bool SetFileChangeDetection(bool enabled, uint32_t intervalInSeconds);
	virtual bool DoFileChangeCheck();

public:
	virtual bool Open(bool readable, bool writable);
	virtual bool IsOpen() const;

	virtual bool Close();
	virtual bool Flush();

	virtual uint32_t GetSegmentID() const;
	virtual bool SetSegmentID(uint32_t const& id);

	virtual uint32_t GetRecordCount(cIGZPersistResourceKeyFilter* filter);
	virtual uint32_t GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter);

	virtual bool Init(uint32_t segmentID, cIGZString const* path, bool allowOpen);

public:
	virtual void FinalRelease();

	virtual bool DoTestForRecord(cGZPersistResourceKey const& key);
	virtual int32_t DoGetRecordSize(cGZPersistResourceKey const& key);
	virtual bool DoOpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t flags);
	virtual bool DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr);
	virtual bool DoCloseRecord(cIGZPersistDBRecord* record);
	virtual bool DoAbortRecord(cIGZPersistDBRecord* record);
	virtual bool DoDeleteRecord(cGZPersistResourceKey const& key);
	virtual uint32_t DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length);
	virtual bool DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length);

protected:
	virtual bool DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr);
	virtual bool OpenRecordInternal(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t mode);
	virtual bool BuildKeyList();
	virtual bool AddFile(cRZString const& fileName, bool allowNewKey);
	virtual bool getFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension);
	virtual bool createFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension);

protected:
	static void CreateDBSegmentDirectoryFilesCallback(cIGZString const& name, cIGZString const& value, void* context);

	void CloseOpenRecords();
	void StartFileChangeCheck();

protected:
	struct RecordData
	{
		RecordData() : fileName(), openRecords() { }
		RecordData(cRZString const& filename) : fileName(filename), openRecords() { }

		typedef std::list<cRZAutoRefCount<cGZDBSDFRecord> > tOpenList;

		cRZString fileName;
		tOpenList openRecords;

		cGZDBSDFRecord* FindRecord(cIGZPersistDBRecord* needle);
		void EraseRecord(cIGZPersistDBRecord* needle);
	};

	typedef std::map<cGZPersistResourceKey, RecordData> RecordMap;

protected:
	bool mInited;
	bool mCheckDirectory;
	bool mUnknown; // TODO: what is this for?

	uint32_t mInstanceIdCounter;

	uint32_t mOpenMode;
	cRZString mPath;
	cRZDirectory* mDirectory;

	uint32_t mDefaultGroupId;
	uint32_t mSegmentId;

	bool mAllowIdAtStart;
	bool mAllowIdAtEnd;
	bool mAllowTextAsKey;

	cGZRESIDtoExtensionMap mExtensionMap;
	RecordMap mRecordMap;

	bool mFileChangeDetectionEnabled;
	uint32_t mChangeCheckIntervalSeconds;
	cRZCallbackTimer mChangeTimer;
	uint32_t mUnknown2; // TODO
	cRZRandom mRandom;
};