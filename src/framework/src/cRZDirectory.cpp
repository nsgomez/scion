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

#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <Windows.h>
#include "cRZDirectory.h"
#include "cRZFile.h"
#include "RZFindFileUtil.h"
#include "RZPathUtil.h"

cRZDirectory::cRZDirectory(cRZString const& path, bool readDirEntriesImmediately, bool includeCurrentDotDir, bool includeParentDotDir) :
	dirEntries(),
	path(path),
	entryFilter("*"),
	includeCurrentDotDir(includeCurrentDotDir),
	includeParentDotDir(includeParentDotDir)
{
	MakeSurePathEndsWithSeparator();
	if (readDirEntriesImmediately)
	{
		ReadDirectoryEntries();
	}
}

bool cRZDirectory::ChangeDirectory(cRZString const& path, bool readDirEntriesImmediately)
{
	this->path = path;
	if (readDirEntriesImmediately)
	{
		ReadDirectoryEntries();
	}

	return true;
}

uint32_t cRZDirectory::GetDirectoryEntryCount(void)
{
	return entryCount;
}

cRZDirectoryEntry* cRZDirectory::GetNthEntry(uint32_t index)
{
	uint32_t i = 0;
	for (std::list<cRZDirectoryEntry>::iterator it = dirEntries.begin(); it != dirEntries.end(); it++)
	{
		if (i == index)
		{
			return &*it;
		}
	}

	return NULL;
}

bool cRZDirectory::ReadDirectoryDirectoryEntriesIntoStringList(std::list<cRZString>& list, uint32_t flags)
{
	return ReadDirectorySpecificEntriesIntoStringList(list, (flags & 3) | RZDirectoryEntryFilterDirectory);
}

bool cRZDirectory::ReadDirectoryFileEntriesIntoStringList(std::list<cRZString>& list)
{
	return ReadDirectorySpecificEntriesIntoStringList(list, RZDirectoryEntryFilterFile);
}

void cRZDirectory::SetNewDirectoryEntryFilter(cRZString const& filter, bool readDirEntriesImmediately)
{
	this->entryFilter = filter;
	if (this->entryFilter.Strlen() == 0)
	{
		this->entryFilter = cRZString("*");
	}

	if (readDirEntriesImmediately)
	{
		ReadDirectoryEntries();
	}
}

void cRZDirectory::AlphabetizeEntryList(std::list<cRZString>& list)
{
	list.sort();
}

bool cRZDirectory::CreateDirectoryA(cRZString const& path)
{
	std::list<cRZString> dirsCreated;
	cRZString tmpPath(path);
	cRZString iteratedPath;
	uint32_t offset = 0;

	if (tmpPath.Strlen() == 0)
	{
		return false;
	}

	if (!IsPathSeparator(tmpPath[tmpPath.Strlen() - 1]))
	{
		tmpPath += '\\';
	}

	uint32_t pathLen = tmpPath.Strlen();
	if (pathLen > 2
		&& tmpPath[0] == '\\'
		&& tmpPath[1] == '\\'
		&& tmpPath[2] != '\\')
	{
		uint32_t separatorCount = 0;
		while (offset < pathLen && separatorCount < 3)
		{
			if (tmpPath[offset] == '\\')
			{
				separatorCount++;
			}

			iteratedPath += tmpPath[offset];
			offset++;
		}
	}

	for (; offset < pathLen; offset++)
	{
		iteratedPath += tmpPath[offset];
		if (IsPathSeparator(tmpPath[offset]) && !DoesDirectoryExist(iteratedPath))
		{
			if (mkdir(iteratedPath.ToChar()) != 0)
			{
				for (std::list<cRZString>::reverse_iterator it = dirsCreated.rbegin(); it != dirsCreated.rend(); it++)
				{
					rmdir(it->ToChar());
				}

				return false;
			}

			dirsCreated.push_back(iteratedPath);
		}
	}

	return true;
}

bool cRZDirectory::DoesAnyEntryExistThatMatchesPattern(cRZString const& pattern)
{
	RZFileFindData* result = RZFileFindFirst(pattern);
	if (result != NULL)
	{
		RZFileFindFinish(result);
	}

	return result != NULL;
}

bool cRZDirectory::DoesDirectoryExist(cRZString const& path)
{
	cRZString pattern(path);
	if (pattern.Strlen() != 0)
	{
		char c = pattern[pattern.Strlen() - 1];
		if (IsPathSeparator(c))
		{
			pattern += '\\';
		}
	}

	pattern += "*";
	return DoesAnyEntryExistThatMatchesPattern(pattern);
}

bool cRZDirectory::RemoveDirectoryA(cRZString const& path, bool recursive)
{
	cRZString tmpPath(path);
	cRZString iteratedPath;
	uint32_t offset = 0;

	if (tmpPath.Strlen() == 0 || !IsPathSeparator(tmpPath[tmpPath.Strlen() - 1]))
	{
		tmpPath += '\\';
	}

	bool succeeded = (::RemoveDirectory(tmpPath.ToChar()) != 0);
	DWORD lastError;

	if (!succeeded && recursive
		&& (lastError = GetLastError()) != ERROR_FILE_NOT_FOUND
		&& lastError != ERROR_PATH_NOT_FOUND
		&& DoesDirectoryExist(tmpPath.ToChar())
		&& lastError != ERROR_WRITE_PROTECT)
	{
		cRZDirectory tmpDir(tmpPath, false, false, false);
		succeeded = true;

		std::list<cRZString> innerEntries;
		if (tmpDir.ReadDirectoryDirectoryEntriesIntoStringList(innerEntries, RZDirectoryEntryFilterDirectory))
		{
			for (std::list<cRZString>::iterator it = innerEntries.begin(); it != innerEntries.end(); it++)
			{
				succeeded = RemoveDirectoryA(*it, true);
				if (!succeeded)
				{
					break;
				}
			}
		}

		innerEntries.clear();

		if (succeeded)
		{
			if (tmpDir.ReadDirectorySpecificEntriesIntoStringList(innerEntries, RZDirectoryEntryFilterFile))
			{
				for (std::list<cRZString>::iterator it = innerEntries.begin(); it != innerEntries.end(); it++)
				{
					succeeded = cRZFile::Remove(*it);
					if (!succeeded)
					{
						break;
					}
				}
			}
		}

		if (succeeded)
		{
			succeeded = (::RemoveDirectory(tmpPath.ToChar()) != 0);
		}
	}

	return succeeded;
}

bool cRZDirectory::SplitDirectoryPath(cRZString const& path, cRZString&, cRZString&)
{
	// TODO
	return false;
}

bool cRZDirectory::DoesDirectoryListContainEntry(cRZString const& name)
{
	for (std::list<cRZDirectoryEntry>::iterator it = dirEntries.begin(); it != dirEntries.end(); it++)
	{
		if (it->name == name)
		{
			return true;
		}
	}

	return false;
}

void cRZDirectory::MakeSurePathEndsWithSeparator(void)
{
	if (path.Strlen() != 0)
	{
		char c = path[path.Strlen() - 1];
		if (IsPathSeparator(c))
		{
			path += '\\';
		}
	}
}

bool cRZDirectory::ReadDirectoryEntries(void)
{
	cRZString fullFilter = path + entryFilter;
	cRZDirectoryEntry entry;

	dirEntries.clear();
	RZFileFindData* findData = RZFileFindFirst(fullFilter);

	if (findData == NULL)
	{
		return false;
	}

	entry.parent = this;

	bool foundCurrentDotDir = false;
	bool foundParentDotDir = false;

	do
	{
		if (!findData->isDirectory)
		{
			entry.flags = RZDirectoryEntryFilterFile;
			entry.name = findData->name;
			dirEntries.push_back(entry);
			entryCount++;
		}
		else
		{
			entry.flags = RZDirectoryEntryFilterDirectory;
			if (findData->name == ".")
			{
				foundCurrentDotDir = true;
				if (includeCurrentDotDir)
				{
					entry.name = findData->name;
					dirEntries.push_back(entry);
					entryCount++;
				}
			}
			else if (findData->name == "..")
			{
				foundParentDotDir = true;
				if (includeParentDotDir)
				{
					entry.name = findData->name;
					dirEntries.push_back(entry);
					entryCount++;
				}
			}
			else
			{
				entry.name = findData->name;
				dirEntries.push_back(entry);
				entryCount++;
			}
		}
	}
	while (RZFileFindNext(findData));
	RZFileFindFinish(findData);

	if (includeCurrentDotDir && !foundCurrentDotDir && entryFilter == "*")
	{
		cRZString name(".");
		if (!DoesDirectoryListContainEntry(name))
		{
			entry.flags = RZDirectoryEntryFilterDirectory;
			entry.name = name;

			dirEntries.push_back(entry);
			entryCount++;
		}
	}

	if (includeParentDotDir && !foundParentDotDir && entryFilter == "*")
	{
		cRZString name("..");
		if (!DoesDirectoryListContainEntry(name))
		{
			entry.flags = RZDirectoryEntryFilterDirectory;
			entry.name = name;

			dirEntries.push_back(entry);
			entryCount++;
		}
	}

	return true;
}

bool cRZDirectory::ReadDirectorySpecificEntriesIntoStringList(std::list<cRZString>& list, uint32_t flags)
{
	cRZString fullFilter = path + entryFilter;
	RZFileFindData* findData = RZFileFindFirst(fullFilter);

	if (findData == NULL)
	{
		return false;
	}

	bool foundCurrentDotDir = false;
	bool foundParentDotDir = false;
 
	do
	{
		if (!findData->isDirectory)
		{
			if (flags & RZDirectoryEntryFilterFile)
			{
				list.push_back(findData->name);
			}
		}
		else if (flags & RZDirectoryEntryFilterDirectory)
		{
			if (findData->name == ".")
			{
				foundCurrentDotDir = true;
				if (flags & RZDirectoryEntryFilterCurrentDir)
				{
					list.push_back(findData->name);
				}
			}
			else if (findData->name == "..")
			{
				foundParentDotDir = true;
				if (flags & RZDirectoryEntryFilterParentDir)
				{
					list.push_back(findData->name);
				}
			}
			else
			{
				list.push_back(findData->name);
			}
		}
	}
	while (RZFileFindNext(findData));
	RZFileFindFinish(findData);

	if ((flags & RZDirectoryEntryFilterParentDir) != 0
		&& !foundParentDotDir
		&& entryFilter == "*")
	{
		list.push_back(cRZString(".."));
	}

	if ((flags & RZDirectoryEntryFilterCurrentDir) != 0
		&& !foundCurrentDotDir
		&& entryFilter == "*")
	{
		list.push_back(cRZString("."));
	}

	return true;
}