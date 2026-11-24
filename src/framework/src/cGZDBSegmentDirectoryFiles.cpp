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

#include "cGZDBSegmentDirectoryFiles.h"
#include "cIGZFile.h"
#include "cIGZPersistResourceKeyFilter.h"
#include "cIGZPersistResourceKeyList.h"
#include "cRZCriticalSectionHolder.h"
#include "RZPlatform.h"

cGZDBSegmentDirectoryFiles::cGZDBSegmentDirectoryFiles() :
	mInited(false),
	mCheckDirectory(false),
	mUnknown(true),
	mInstanceIdCounter(1),
	mOpenMode(0),
	mPath(),
	mDirectory(NULL),
	mDefaultGroupId(0),
	mSegmentId(0),
	mAllowGroupIdFirst(true),
	mAllowInstanceIdFirst(false),
	mAllowTextAsKey(false),
	mExtensionMap(),
	mRecordMap(),
	mFileChangeDetectionEnabled(false),
	mChangeCheckIntervalMs(120),
	mChangeTimer(),
	mRandom(-1)
{
}

cGZDBSegmentDirectoryFiles::~cGZDBSegmentDirectoryFiles()
{
	Shutdown();
}

bool cGZDBSegmentDirectoryFiles::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid != GZIID_cIGZPersistDBSegmentDirectoryFiles)
	{
		return cGZPersistDBSegment::QueryInterface(iid, outPtr);
	}

	*outPtr = static_cast<cIGZPersistDBSegmentDirectoryFiles*>(this);
	AddRef();
	return true;
}

uint32_t cGZDBSegmentDirectoryFiles::AddRef()
{
	return RZThreadSafeAdd(reinterpret_cast<volatile int32_t*>(&refCount), 1);
}

uint32_t cGZDBSegmentDirectoryFiles::Release()
{
	uint32_t newRefCount = RZThreadSafeAdd(reinterpret_cast<volatile int32_t*>(&refCount), -1);
	if (newRefCount > 0)
	{
		return newRefCount;
	}

	FinalRelease();
	return 0;
}

cIGZPersistDBSegment* cGZDBSegmentDirectoryFiles::AsIDBSegment()
{
	return static_cast<cIGZPersistDBSegment*>(this);
}

bool cGZDBSegmentDirectoryFiles::Init()
{
	if (!mInited)
	{
		mInited = true;
	}

	return true;
}

bool cGZDBSegmentDirectoryFiles::Shutdown()
{
	if (!mInited)
	{
		return true;
	}

	mInited = false;
	mChangeTimer.Cancel();

	if (mOpenMode & GZFileAccessReadWrite)
	{
		Close();
	}

	mInstanceIdCounter = 1;
	mPath.clear();
	mDefaultGroupId = 0;
	mSegmentId = 0;
	mAllowGroupIdFirst = true;
	mAllowInstanceIdFirst = false;

	if (mDirectory != NULL)
	{
		delete mDirectory;
		mDirectory = NULL;
	}

	mExtensionMap.EraseAll();
	mRecordMap.clear();
	return true;
}

cIGZString const& cGZDBSegmentDirectoryFiles::Path() const
{
	return mPath;
}

void cGZDBSegmentDirectoryFiles::GetPath(cIGZString& outPath) const
{
	outPath.FromChar(mPath.Data(), mPath.Strlen());
}

bool cGZDBSegmentDirectoryFiles::SetPath(cIGZString const& path)
{
	// TODO
	return false;
}

void cGZDBSegmentDirectoryFiles::SetDefaultGroupId(uint32_t groupId)
{
	mDefaultGroupId = groupId;
}

uint32_t cGZDBSegmentDirectoryFiles::GroupId() const
{
	return mDefaultGroupId;
}

cIGZPersistRESIDtoExtensionMap& cGZDBSegmentDirectoryFiles::ExtensionMap()
{
	return mExtensionMap;
}

bool cGZDBSegmentDirectoryFiles::GetRESIDtoExtensionMap(cIGZPersistRESIDtoExtensionMap** out)
{
	cGZRESIDtoExtensionMap* clone = new cGZRESIDtoExtensionMap();
	clone->AddRef();
	clone->Copy(&mExtensionMap);

	*out = clone;
	return true;
}

bool cGZDBSegmentDirectoryFiles::AddExtensionAssociation(uint32_t resId, cIGZString const& extension)
{
	return mExtensionMap.AddRESIDtoExtensionAssociation(resId, extension);
}

void cGZDBSegmentDirectoryFiles::SetAllowedIDPositions(bool allowGroupFirst, bool allowInstanceFirst)
{
	mAllowGroupIdFirst = allowGroupFirst;
	mAllowInstanceIdFirst = allowInstanceFirst;
}

void cGZDBSegmentDirectoryFiles::SetAllowTextAsKey(bool allowTextAsKey)
{
	mAllowTextAsKey = allowTextAsKey;
}

bool cGZDBSegmentDirectoryFiles::GetFileNameFromKey(cGZPersistResourceKey const& key, cIGZString& fileName) const
{
	RecordMap::const_iterator it = mRecordMap.find(key);
	if (it != mRecordMap.end())
	{
		fileName.Copy(it->second.fileName);
		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::GetFilePathFromKey(cGZPersistResourceKey const& key, cIGZString& filePath) const
{
	cRZString fileName;
	if (GetFileNameFromKey(key, fileName))
	{
		filePath.Copy(mPath);
		filePath.Append(fileName.Data(), fileName.Strlen());
		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::GetKeyFromFileName(cIGZString const& fileName, cIGZString const& extension, cGZPersistResourceKey& key) const
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::IsFileNameValid(cIGZString const& fileName) const
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::GenerateNewKey(uint32_t type, cGZPersistResourceKey& key)
{
	if (!mInited)
	{
		return false;
	}

	key.group = mDefaultGroupId;
	key.type = type;

	if (mInstanceIdCounter == 0xFFFFFFFF)
	{
		return false;
	}

	do
	{
		key.instance = mInstanceIdCounter;
		++mInstanceIdCounter;

		if (mRecordMap.find(key) == mRecordMap.end())
		{
			return true;
		}
	}
	while (mInstanceIdCounter != 0xFFFFFFFF);

	return false;
}

uint32_t cGZDBSegmentDirectoryFiles::EnumerateRecordFiles(cIGZString** names, uint32_t maxCount)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count = 0;

	if (names != NULL)
	{
		RecordMap::iterator end = mRecordMap.end();
		for (RecordMap::iterator it = mRecordMap.begin(); it != end; it++)
		{
			if (count >= maxCount)
			{
				break;
			}

			if (names[count] == NULL)
			{
				break;
			}

			names[count]->FromChar(it->second.fileName.Data(), it->second.fileName.Strlen());
			++count;
		}
	}
	else
	{
		count = mRecordMap.size();
	}

	return count;
}

bool cGZDBSegmentDirectoryFiles::GetFileChangeDetection() const
{
	return mFileChangeDetectionEnabled;
}

bool cGZDBSegmentDirectoryFiles::SetFileChangeDetection(bool enabled, uint32_t intervalInMs)
{
	if (mFileChangeDetectionEnabled != enabled)
	{
		if (enabled)
		{
			if (mInited)
			{
				StartFileChangeCheck();
			}
		}
		else
		{
			mChangeTimer.Cancel();
		}

		mFileChangeDetectionEnabled = enabled;
	}

	mChangeCheckIntervalMs = intervalInMs;
	if (mChangeCheckIntervalMs < 15)
	{
		mChangeCheckIntervalMs = 15;
	}

	return true;
}

bool cGZDBSegmentDirectoryFiles::DoFileChangeCheck()
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::Open(bool readable, bool writable)
{
	bool result = false;
	if (mOpenMode & GZFileAccessWrite)
	{
		result = true;
	}
	else if (mInited && !mPath.empty() && (readable || writable))
	{
		mOpenMode = 0;
		if (readable)
		{
			mOpenMode = GZFileAccessRead;
		}

		if (writable)
		{
			mOpenMode |= GZFileAccessWrite;
			if (mCheckDirectory && !cRZDirectory::DoesDirectoryExist(mPath))
			{
				result = false;
				mOpenMode = 0;
				return result;
			}
		}

		if (mDirectory == NULL)
		{
			mDirectory = new cRZDirectory(mPath, false /* readDirEntriesImmediately */, false /* includeCurrentDotDir */, false /* includeParentDotDir */);
		}

		// TODO: cGZRegistry is not implemented yet
		/*cRZSysServPtr<cIGZRegistry, GZIID_cIGZRegistry, RZSRVID_cGZRegistry> registry;
		if (registry != NULL && !registry->EnumCategory("CN_DBSDF", CreateDBSegmentDirectoryFilesCallback, this))
		{
			return false;
		}*/

		result = BuildKeyList();
		if (result && mFileChangeDetectionEnabled)
		{
			StartFileChangeCheck();
		}
	}
	else
	{
		mOpenMode = 0;
	}

	return result;
}

bool cGZDBSegmentDirectoryFiles::IsOpen() const
{
	return (mOpenMode & GZFileAccessReadWrite) != 0;
}

bool cGZDBSegmentDirectoryFiles::Close()
{
	if ((mOpenMode & GZFileAccessReadWrite) == 0)
	{
		return true;
	}

	CloseOpenRecords();

	mOpenMode = 0;
	mExtensionMap.EraseAll();
	mRecordMap.clear();

	if (mDirectory != NULL)
	{
		delete mDirectory;
		mDirectory = NULL;
	}

	return true;
}

bool cGZDBSegmentDirectoryFiles::Flush()
{
	return true;
}

uint32_t cGZDBSegmentDirectoryFiles::GetSegmentID() const
{
	return mSegmentId;
}

bool cGZDBSegmentDirectoryFiles::SetSegmentID(uint32_t const& id)
{
	mSegmentId = id;
	if (mDefaultGroupId == 0)
	{
		mDefaultGroupId = id;
	}

	return true;
}

uint32_t cGZDBSegmentDirectoryFiles::GetRecordCount(cIGZPersistResourceKeyFilter* filter)
{
	return GetResourceKeyList(NULL, filter);
}

uint32_t cGZDBSegmentDirectoryFiles::GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter)
{
	uint32_t count = 0;

	RecordMap::iterator end = mRecordMap.end();
	for (RecordMap::iterator it = mRecordMap.begin(); it != end; it++)
	{
		cGZPersistResourceKey const& key = it->first;
		if (filter != NULL && !filter->IsKeyIncluded(key))
		{
			continue;
		}

		if (list != NULL)
		{
			list->Insert(key);
		}

		count++;
	}

	return count;
}

uint32_t cGZDBSegmentDirectoryFiles::GetResourceKeyList(cIGZPersistResourceKeyList& list)
{
	// TODO
	return 0;
}

bool cGZDBSegmentDirectoryFiles::Init(uint32_t segmentID, cIGZString const* path, bool allowOpen)
{
	// TODO
	return false;
}

void cGZDBSegmentDirectoryFiles::FinalRelease()
{
	Shutdown();
	cGZPersistDBSegment::FinalRelease();
}

bool cGZDBSegmentDirectoryFiles::DoTestForRecord(cGZPersistResourceKey const& key)
{
	return mRecordMap.find(key) != mRecordMap.end();
}

int32_t cGZDBSegmentDirectoryFiles::DoGetRecordSize(cGZPersistResourceKey const& key)
{
	// TODO
	return -1;
}

bool cGZDBSegmentDirectoryFiles::DoOpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t flags)
{
	if (flags & GZFileAccessWrite)
	{
		return OpenRecordInternal(key, outPtr, GZFileAccessWrite);
	}

	if (flags & GZFileAccessRead)
	{
		return OpenRecordInternal(key, outPtr, GZFileAccessRead);
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::DoCreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	return OpenRecordInternal(key, outPtr, GZFileAccessReadWrite);
}

bool cGZDBSegmentDirectoryFiles::DoCloseRecord(cIGZPersistDBRecord* record)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::DoAbortRecord(cIGZPersistDBRecord* recordPtr)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::DoDeleteRecord(cGZPersistResourceKey const& key)
{
	// TODO
	return false;
}

uint32_t cGZDBSegmentDirectoryFiles::DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	return OpenRecordInternal(key, outPtr, GZFileAccessWrite);
}

bool cGZDBSegmentDirectoryFiles::OpenRecordInternal(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t mode)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::BuildKeyList()
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::AddFile(cRZString const& fileName, bool allowNewKey)
{
	// TODO
	return false;
}

bool cGZDBSegmentDirectoryFiles::getFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension)
{
	if (mExtensionMap.RESIDToExtension(key.group, outExtension))
	{
		if (key.group == mDefaultGroupId)
		{
			outName.Sprintf("%8.8x", key.instance);
		}
		else
		{
			outName.Sprintf("%8.8x%8.8x", key.instance, key.group);
		}
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::createFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension)
{
	return getFileNameFromKey(key, outName, outExtension);
}

void cGZDBSegmentDirectoryFiles::CloseOpenRecords()
{
	// TODO
}

void cGZDBSegmentDirectoryFiles::StartFileChangeCheck()
{
	// TODO
}