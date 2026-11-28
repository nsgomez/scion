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

#include <cstdlib>
#include "cGZDBSegmentDirectoryFiles.h"
#include "cIGZFile.h"
#include "cIGZFileSystem.h"
#include "cIGZLanguageManager.h"
#include "cIGZPersistResourceKeyFilter.h"
#include "cIGZPersistResourceKeyList.h"
#include "cIGZRegistry.h"
#include "cRZCriticalSectionHolder.h"
#include "cRZSysServPtr.h"
#include "nRZHash.h"
#include "RZPathUtil.h"
#include "RZPlatform.h"
#include "RZStringUtil.h"

cGZDBSDFRecord* cGZDBSegmentDirectoryFiles::RecordData::FindRecord(cIGZPersistDBRecord* needle)
{
	tOpenList::iterator it = openRecords.begin();
	while (it != openRecords.end())
	{
		cGZDBSDFRecord* record = *it;
		if (needle == static_cast<cIGZPersistDBRecord*>(record))
		{
			return record;
		}

		++it;
	}

	return NULL;
}

void cGZDBSegmentDirectoryFiles::RecordData::EraseRecord(cIGZPersistDBRecord* needle)
{
	tOpenList::iterator it = openRecords.begin();
	while (it != openRecords.end())
	{
		cGZDBSDFRecord* record = *it;
		if (needle == static_cast<cIGZPersistDBRecord*>(record))
		{
			openRecords.erase(it);
			return;
		}

		++it;
	}
}

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
	mAllowIdAtStart(true),
	mAllowIdAtEnd(false),
	mAllowTextAsKey(false),
	mExtensionMap(),
	mRecordMap(),
	mFileChangeDetectionEnabled(false),
	mChangeCheckIntervalSeconds(120),
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
	mChangeTimer.Stop();

	if (mOpenMode & GZFileAccessReadWrite)
	{
		Close();
	}

	mInstanceIdCounter = 1;
	mPath.clear();
	mDefaultGroupId = 0;
	mSegmentId = 0;
	mAllowIdAtStart = true;
	mAllowIdAtEnd = false;

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
	outPath.FromChar(mPath.c_str(), mPath.size());
}

bool cGZDBSegmentDirectoryFiles::SetPath(cIGZString const& path)
{
	if ((mOpenMode & (GZFileAccessRead | GZFileAccessWrite)) == 0)
	{
		mPath = path;
		if (!IsPathSeparator(mPath[mPath.size() - 1]))
		{
			mPath += '\\';
		}

		return true;
	}

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

void cGZDBSegmentDirectoryFiles::SetAllowedIDPositions(bool allowIdAtStart, bool allowIdAtEnd)
{
	mAllowIdAtStart = allowIdAtStart;
	mAllowIdAtEnd = allowIdAtEnd;
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
		filePath.Append(fileName.c_str(), fileName.size());
		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::GetKeyFromFileName(cIGZString const& fileName, cIGZString const& extension, cGZPersistResourceKey& key) const
{
	uint32_t group = mDefaultGroupId;
	uint32_t fileNameLen = fileName.Strlen();
	uint32_t instance;

	if (fileNameLen >= 8)
	{
		if (mAllowIdAtStart)
		{
			char const* chars = fileName.ToChar();
			int maxDigits = 16;

			if (fileNameLen < maxDigits)
			{
				maxDigits = fileNameLen;
			}

			int digits = 0;
			while (digits < maxDigits && isxdigit(chars[digits]))
			{
				++digits;
			}

			if (digits >= 8)
			{
				instance = strtoul(cRZString(chars, 8).ToChar(), NULL, 16);
				if (fileNameLen >= 16 && digits >= 16)
				{
					group = strtoul(cRZString(chars + 8, 8).ToChar(), NULL, 16);
				}

				goto keyResolved;
			}
		}

		if (mAllowIdAtEnd)
		{
			int pos = fileNameLen - 1;
			int limit = fileNameLen < 16 ? 0 : fileNameLen - 16;
			char const* chars = fileName.ToChar();

			while (pos >= limit && isxdigit(chars[pos]))
			{
				--pos;
			}

			uint32_t digits = fileNameLen - pos - 1;
			if (digits >= 8)
			{
				if (digits >= 16)
				{
					group = strtoul(cRZString(chars + fileNameLen - 8, 8).ToChar(), NULL, 16);
					instance = strtoul(cRZString(chars + fileNameLen - 16, 8).ToChar(), NULL, 16);
				}
				else
				{
					instance = strtoul(cRZString(chars + fileNameLen - 8, 8).ToChar(), NULL, 16);
				}

				goto keyResolved;
			}
		}
	}

	if (!mAllowTextAsKey)
	{
		return false;
	}

	instance = nRZHash::CRC24(fileName.ToChar(), false /* caseInsensitive */) | 0xff000000;

keyResolved:
	if (const_cast<cGZRESIDtoExtensionMap&>(mExtensionMap).ExtensionToRESID(extension, key))
	{
		key.group = group;
		key.instance = instance;

		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::IsFileNameValid(cIGZString const& fileName) const
{
	cRZString name;
	cRZString extension;
	cGZPersistResourceKey key;

	cRZFile::GetPathName(fileName, name, false);
	cRZFile::GetPathExtension(fileName, extension);

	if (extension.size() != 0 && extension[0] == '.')
	{
		extension.erase(0, 1);
	}

	return GetKeyFromFileName(name, extension, key);
}

bool cGZDBSegmentDirectoryFiles::GenerateNewKey(uint32_t type, cGZPersistResourceKey& key)
{
	if (mInited)
	{
		key.group = mDefaultGroupId;
		key.type = type;

		while (mInstanceIdCounter != 0xFFFFFFFF)
		{
			key.instance = mInstanceIdCounter;
			++mInstanceIdCounter;

			if (mRecordMap.find(key) == mRecordMap.end())
			{
				return true;
			}
		}
	}

	return false;
}

uint32_t cGZDBSegmentDirectoryFiles::EnumerateRecordFiles(cIGZString** names, uint32_t maxCount)
{
	cRZCriticalSectionHolder lock(criticalSection);
	uint32_t count = 0;

	if (names != NULL)
	{
		RecordMap::iterator end = mRecordMap.end();
		RecordMap::iterator it = mRecordMap.begin();

		while (it != end)
		{
			if (count >= maxCount)
			{
				break;
			}

			if (names[count] == NULL)
			{
				break;
			}

			names[count]->FromChar(it->second.fileName.c_str(), it->second.fileName.size());
			++count;
			it++;
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

bool cGZDBSegmentDirectoryFiles::SetFileChangeDetection(bool enabled, uint32_t intervalInSeconds)
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
			mChangeTimer.Stop();
		}

		mFileChangeDetectionEnabled = enabled;
	}

	mChangeCheckIntervalSeconds = intervalInSeconds;
	if (mChangeCheckIntervalSeconds < 15)
	{
		mChangeCheckIntervalSeconds = 15;
	}

	return true;
}

bool cGZDBSegmentDirectoryFiles::DoFileChangeCheck()
{
	bool changed = false;
	if (mInited)
	{
		cRZDirectory dir(mPath, false /* readDirEntriesImmediately */, false /* includeCurrentDotDir */, false /* includeParentDotDir */);
		std::list<cRZString> filesFromDirectory;

		if (dir.ReadDirectoryFileEntriesIntoStringList(filesFromDirectory))
		{
			uint32_t numFilesFromRecords = EnumerateRecordFiles(NULL, 0);
			uint32_t numFilesFromDirectory = 0;

			std::list<cRZString>::iterator dirIt = filesFromDirectory.begin();
			if (filesFromDirectory.empty())
			{
				goto end;
			}

			while (dirIt != filesFromDirectory.end())
			{
				cRZString& file = *dirIt;
				if (IsFileNameValid(file))
				{
					file.MakeLower();
					++numFilesFromDirectory;
					++dirIt;
				}
				else
				{
					dirIt = filesFromDirectory.erase(dirIt);
				}
			}

			if (numFilesFromRecords == 0 || numFilesFromDirectory == 0)
			{
				goto end;
			}

			cIGZString** fileNamesFromRecords = new cIGZString*[numFilesFromRecords];
			for (uint32_t i = 0; i < numFilesFromRecords; i++)
			{
				fileNamesFromRecords[i] = new cRZString();
			}

			EnumerateRecordFiles(fileNamesFromRecords, numFilesFromRecords);
			for (uint32_t i = 0; i < numFilesFromRecords; i++)
			{
				static_cast<cRZString*>(fileNamesFromRecords[i])->MakeLower();
			}

			uint32_t numMatches = 0;
			std::list<cRZString>::iterator dirIt2 = filesFromDirectory.begin();

			if (!filesFromDirectory.empty())
			{
				while (dirIt2 != filesFromDirectory.end())
				{
					int32_t i = 0;
					while (i < (int32_t)numFilesFromRecords)
					{
						if (fileNamesFromRecords[i] != NULL
							&& *static_cast<cRZString*>(fileNamesFromRecords[i]) == *dirIt2)
						{
							break;
						}

						++i;
					}

					if (i < (int32_t)numFilesFromRecords)
					{
						delete fileNamesFromRecords[i];
						fileNamesFromRecords[i] = NULL;

						dirIt2 = filesFromDirectory.erase(dirIt2);
						++numMatches;
					}
					else
					{
						++dirIt2;
					}
				}
			}

			if (numMatches < numFilesFromRecords)
			{
				changed = true;
				for (uint32_t i = 0; i < numFilesFromRecords; i++)
				{
					if (fileNamesFromRecords[i] != NULL)
					{
						AddFile(*static_cast<cRZString*>(fileNamesFromRecords[i]), false /* allowNewKey */);
					}
				}
			}

			if (!filesFromDirectory.empty())
			{
				changed = true;
				for (std::list<cRZString>::iterator it = filesFromDirectory.begin(); it != filesFromDirectory.end(); ++it)
				{
					AddFile(cRZString(*it), true /* allowNewKey */);
				}
			}

			for (uint32_t i = 0; i < numFilesFromRecords; i++)
			{
				if (fileNamesFromRecords[i] != NULL)
				{
					delete static_cast<cRZString*>(fileNamesFromRecords[i]);
				}
			}

			delete[] fileNamesFromRecords;
		}

end:
		if (mFileChangeDetectionEnabled)
		{
			uint32_t interval = mChangeCheckIntervalSeconds;
			int32_t delayInSeconds = mRandom.RandomSint32RangeUniform(interval, interval * 2);
			mChangeTimer.StartOneShot((uint32_t)delayInSeconds * 1000, this);
		}
	}

	return changed;
}

void cGZDBSegmentDirectoryFiles::CreateDBSegmentDirectoryFilesCallback(cIGZString const& name, cIGZString const& value, void* context)
{
	cGZDBSegmentDirectoryFiles* self = static_cast<cGZDBSegmentDirectoryFiles*>(context);

	if (RZAtoUint32HexA(name.ToChar()) != self->mDefaultGroupId)
	{
		return;
	}

	cRZString valueStr(value);
	cRZString token;
	cRZString extension;

	do
	{
		valueStr.SplitTokenDelimited(',', token);
		token.Trim();

		if (token.length() != 0)
		{
			token.MakeLower();
			extension = token.SplitToken(":");

			uint32_t resId = RZAtoUint32HexA(extension.ToChar());
			uint32_t pos = token.find(":filenameasinstanceid");

			if (pos != std::string::npos)
			{
				self->SetAllowTextAsKey(true);
				token.erase(pos);
			}

			self->mExtensionMap.AddRESIDtoExtensionAssociation(resId, token);
		}
	}
	while (!token.empty() && !valueStr.empty());
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

		cRZSysServPtr<cIGZRegistry, GZIID_cIGZRegistry, RZSRVID_cGZRegistry> registry;
		if (registry != NULL && !registry->EnumCategory("CN_DBSDF", CreateDBSegmentDirectoryFilesCallback, this))
		{
			return false;
		}

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

bool cGZDBSegmentDirectoryFiles::Init(uint32_t segmentID, cIGZString const* path, bool allowOpen)
{
	bool result = false;
	if (!mInited)
	{
		SetSegmentID(segmentID);

		mInited = true;
		mCheckDirectory = true;
		mUnknown = false;
		mPath = *path;

		cRZSysServPtr<cIGZRegistry, GZIID_cIGZRegistry, RZSRVID_cGZRegistry> registry;
		if (registry != NULL && !registry->EnumCategory("CN_DBSDF", CreateDBSegmentDirectoryFilesCallback, this))
		{
			return false;
		}

		if (mPath.find("%DataDirectory%") < mPath.length())
		{
			cRZSysServPtr<cIGZFileSystem, GZIID_cIGZFileSystem, RZSRVID_cGZFileSystem> fileSystem;
			if (fileSystem != NULL)
			{
				cRZString dataDirectory(fileSystem->DataDirectory());
				mPath.SplitToken("%DataDirectory%");
				mPath = cRZString(dataDirectory + mPath);
			}
		}

		if (mPath.find("%Language%") < mPath.length())
		{
			cRZSysServPtr<cIGZLanguageManager, GZIID_cIGZLanguageManager, RZSRVID_cRZLanguageManager> languageManager;
			if (languageManager != NULL)
			{
				cRZString languageName;
				cRZString prefix(mPath.SplitToken("%Language%"), 0, -1);

				languageManager->GetLanguageDirectoryName(languageName, kGZLanguage_Current);
				mPath = cRZString(prefix + languageName + mPath);
			}
		}

		if (SetPath(mPath))
		{
			result = Open(true /* readable */, true /* writable */);
		}

		if (!result)
		{
			Shutdown();
		}
	}

	return result;
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
	int32_t size = -1;
	if (mInited)
	{
		RecordMap::iterator it = mRecordMap.find(key);
		if (it != mRecordMap.end())
		{
			RecordData::tOpenList::iterator openIt = it->second.openRecords.begin();
			RecordData::tOpenList::iterator openEnd = it->second.openRecords.end();

			while (openIt != openEnd)
			{
				cGZDBSDFRecord* record = *openIt;
				uint32_t mode = record->mMode;

				if (mode == GZFileAccessWrite || mode == GZFileAccessReadWrite)
				{
					size = record->GetSize();
				}

				++openIt;
			}

			if (size == -1)
			{
				cRZString const& fileName = it->second.fileName;
				cRZString path(mPath + fileName);

				size = cRZFile::GetLength(path);
			}
		}
	}

	return size;
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
	bool result = false;
	cGZPersistResourceKey key = record->GetKey();
	RecordMap::iterator it = mRecordMap.find(key);

	if (it != mRecordMap.end())
	{
		RecordData& data = it->second;
		cGZDBSDFRecord* recordFromData = data.FindRecord(record);

		if (recordFromData != NULL)
		{
			recordFromData->CloseInternal();
			data.EraseRecord(static_cast<cIGZPersistDBRecord*>(recordFromData));
			result = true;
		}
	}

	return result;
}

bool cGZDBSegmentDirectoryFiles::DoAbortRecord(cIGZPersistDBRecord* record)
{
	bool result = false;
	cGZPersistResourceKey key = record->GetKey();
	RecordMap::iterator it = mRecordMap.find(key);

	if (it != mRecordMap.end())
	{
		RecordData& data = it->second;
		cGZDBSDFRecord* recordFromData = data.FindRecord(record);

		if (recordFromData != NULL)
		{
			recordFromData->AbortInternal();
			data.EraseRecord(static_cast<cIGZPersistDBRecord*>(recordFromData));
			result = true;
		}
	}

	return result;
}

bool cGZDBSegmentDirectoryFiles::DoDeleteRecord(cGZPersistResourceKey const& key)
{
	bool result = false;
	if (mInited)
	{
		RecordMap::iterator it = mRecordMap.find(key);
		if (it != mRecordMap.end())
		{
			RecordData& data = it->second;
			if (data.openRecords.empty())
			{
				cRZString path(mPath + data.fileName);

				mRecordMap.erase(it);
				cRZFile::Remove(path);

				result = true;
			}
		}
	}

	return result;
}

uint32_t cGZDBSegmentDirectoryFiles::DoReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
{
	uint32_t size = -1;
	uint32_t bytesRead = 0;

	if (mInited)
	{
		RecordMap::iterator it = mRecordMap.find(key);
		if (it != mRecordMap.end())
		{
			RecordData::tOpenList const& openRecords = it->second.openRecords;

			if (std::distance(openRecords.begin(), openRecords.end()) == 1
				&& ((*openRecords.begin())->mMode == GZFileAccessWrite || (*openRecords.begin())->mMode == GZFileAccessReadWrite))
			{
				size = -1;
				bytesRead = 0;
			}
			else
			{
				cRZString path(mPath + it->second.fileName);
				if (data != NULL)
				{
					cRZFile file(path);
					if (file.Open(GZFileAccessRead, GZFileOpenExistingOnly, GZFileShareRead))
					{
						uint32_t fileLength = (uint32_t)file.Length();
						length = std::min<uint32_t>(length, fileLength);

						file.ReadWithCount(data, length);
						bytesRead = length;
						size = length;

						file.Close();
					}
				}
				else
				{
					bytesRead = 0;
					size = cRZFile::GetLength(path);
				}
			}
		}
	}

	length = bytesRead;
	return size;
}

bool cGZDBSegmentDirectoryFiles::DoWriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length)
{
	return false;
}

bool cGZDBSegmentDirectoryFiles::DoOpenRecordForWrite(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr)
{
	return OpenRecordInternal(key, outPtr, GZFileAccessWrite);
}

bool cGZDBSegmentDirectoryFiles::OpenRecordInternal(cGZPersistResourceKey const& key, cIGZPersistDBRecord** outPtr, uint32_t mode)
{
	if (!mInited || mode == 0)
	{
		return false;
	}

	RecordMap::iterator it = mRecordMap.find(key);
	if (it == mRecordMap.end())
	{
		if (mode == GZFileAccessRead)
		{
			return false;
		}
	}
	else if (!it->second.openRecords.empty())
	{
		if (mode == GZFileAccessReadWrite || mode == GZFileAccessWrite)
		{
			return false;
		}

		if ((*it->second.openRecords.begin())->mMode != GZFileAccessRead)
		{
			return false;
		}
	}

	cRZString fileName;
	cRZString fullPath;

	if (mode == GZFileAccessReadWrite)
	{
		cRZString extension;
		createFileNameFromKey(key, fileName, extension);
		fileName += '.';
		fileName += extension;
	}
	else
	{
		GetFileNameFromKey(key, fileName);
	}

	fullPath = mPath + fileName;

	cGZDBSDFRecord* record = new cGZDBSDFRecord(this);
	record->AddRef();

	if (record->Init(key, fullPath, (GZFileAccessFlags)mode))
	{
		if (it == mRecordMap.end())
		{
			RecordData& data = mRecordMap[key];
			data.openRecords.push_back(cRZAutoRefCount<cGZDBSDFRecord>(record));
			data.fileName = fileName;
		}
		else
		{
			it->second.openRecords.push_back(cRZAutoRefCount<cGZDBSDFRecord>(record));
		}

		if (record->QueryInterface(GZIID_cIGZPersistDBRecord, reinterpret_cast<void**>(outPtr)))
		{
			record->Release();
			return true;
		}

		mRecordMap[key].EraseRecord(static_cast<cIGZPersistDBRecord*>(record));
	}

	record->Release();
	return false;
}

bool cGZDBSegmentDirectoryFiles::BuildKeyList()
{
	char drive[_MAX_DRIVE + 1];
	cRZString fileName;
	cRZString extension;
	std::list<cRZString> fileList;

	cGZPersistResourceKey key;

	char ext[_MAX_EXT];
	char fname[_MAX_FNAME];
	char dir[_MAX_DIR];

	if (cRZDirectory::DoesDirectoryExist(mPath))
	{
		mDirectory->ReadDirectoryFileEntriesIntoStringList(fileList);

		std::list<cRZString>::iterator end = fileList.end();
		for (std::list<cRZString>::iterator it = fileList.begin(); it != end; it++)
		{
			_splitpath(it->ToChar(), drive, dir, fname, ext);
			fileName = cRZString(fname);

			if (ext[0] != '\0')
			{
				extension.FromChar(ext + 1);
			}

			if (GetKeyFromFileName(fileName, extension, key) && mRecordMap.find(key) == mRecordMap.end())
			{
				RecordData recordData(*it);
				mRecordMap.insert(RecordMap::value_type(key, recordData));
			}
		}

		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::AddFile(cRZString const& fileName, bool allowNewKey)
{
	bool result = false;
	if (allowNewKey)
	{
		cRZString name;
		cRZString extension;
		cGZPersistResourceKey key;

		cRZFile::GetPathName(fileName, name, false /* includeSeparator */);
		cRZFile::GetPathExtension(fileName, extension);

		if (!extension.empty())
		{
			extension.erase(0, 1);
		}

		if (GetKeyFromFileName(name, extension, key) && mRecordMap.find(key) == mRecordMap.end())
		{
			mRecordMap.insert(RecordMap::value_type(key, RecordData(fileName)));
		}
	}
	else
	{
		RecordMap::iterator end = mRecordMap.end();
		for (RecordMap::iterator it = mRecordMap.begin(); it != end; it++)
		{
			RecordData& data = it->second;
			if (data.openRecords.empty())
			{
				cRZString lowerCase(data.fileName);
				lowerCase.MakeLower();

				if (lowerCase == fileName)
				{
					mRecordMap.erase(it);
					result = true;
					break;
				}
			}
		}
	}

	return result;
}

bool cGZDBSegmentDirectoryFiles::getFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension)
{
	if (mExtensionMap.RESIDToExtension(key.type, outExtension))
	{
		if (key.group == mDefaultGroupId)
		{
			outName.Sprintf("%8.8x", key.instance);
		}
		else
		{
			outName.Sprintf("%8.8x%8.8x", key.instance, key.group);
		}

		return true;
	}

	return false;
}

bool cGZDBSegmentDirectoryFiles::createFileNameFromKey(cGZPersistResourceKey const& key, cRZString& outName, cRZString& outExtension)
{
	return getFileNameFromKey(key, outName, outExtension);
}

void cGZDBSegmentDirectoryFiles::CloseOpenRecords()
{
	RecordMap::iterator end = mRecordMap.end();
	for (RecordMap::iterator it = mRecordMap.begin(); it != end; ++it)
	{
		RecordData::tOpenList& openRecords = it->second.openRecords;
		if (!openRecords.empty())
		{
			RecordData::tOpenList::iterator openIt = openRecords.begin();
			RecordData::tOpenList::iterator openEnd = openRecords.end();

			while (openIt != openEnd)
			{
				(*openIt)->CloseInternal();
				++openIt;
			}

			openRecords.clear();
		}
	}
}

static void sFileChangeCheckCallback(void* context)
{
	static_cast<cGZDBSegmentDirectoryFiles*>(context)->DoFileChangeCheck();
}

void cGZDBSegmentDirectoryFiles::StartFileChangeCheck()
{
	if (mChangeTimer.IsRunning())
	{
		return;
	}

	uint32_t interval = mChangeCheckIntervalSeconds;
	int32_t delayInSeconds = mRandom.RandomSint32RangeUniform(interval, interval * 2);

	mChangeTimer.SetCallbackFunction(sFileChangeCheckCallback, this);
	mChangeTimer.StartOneShot((uint32_t)delayInSeconds * 1000, this);
}