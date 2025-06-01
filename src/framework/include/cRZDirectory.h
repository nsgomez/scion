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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <list>
#include "cRZString.h"

 class cRZDirectory;

 enum RZDirectoryEntryFilterFlags
 {
	 RZDirectoryEntryFilterCurrentDir = 1,
	 RZDirectoryEntryFilterParentDir = 2,
	 RZDirectoryEntryFilterDirectory = 4,
	 RZDirectoryEntryFilterFile = 8,
 };

 struct cRZDirectoryEntry
 {
 public:
	 cRZDirectory* parent;
	 cRZString name;
	 uint32_t flags;
 };

class cRZDirectory
{
public:
	cRZDirectory(cRZString const& path, bool readDirEntriesImmediately, bool includeCurrentDotDir, bool includeParentDotDir);

public:
	bool ChangeDirectory(cRZString const& path, bool readDirEntriesImmediately);
	uint32_t GetDirectoryEntryCount(void);
	cRZDirectoryEntry* GetNthEntry(uint32_t index);
	bool ReadDirectoryDirectoryEntriesIntoStringList(std::list<cRZString>& list, uint32_t flags);
	bool ReadDirectoryFileEntriesIntoStringList(std::list<cRZString>& list);
	void SetNewDirectoryEntryFilter(cRZString const& filter, bool readDirEntriesImmediately);

public:
	static void AlphabetizeEntryList(std::list<cRZString>& list);
	static bool CreateDirectoryA(cRZString const& path);
	static bool DoesAnyEntryExistThatMatchesPattern(cRZString const& pattern);
	static bool DoesDirectoryExist(cRZString const& path);
	static bool RemoveDirectoryA(cRZString const& path, bool recursive);
	static bool SplitDirectoryPath(cRZString const& path, cRZString&, cRZString&);

protected:
	bool DoesDirectoryListContainEntry(cRZString const& name);
	void MakeSurePathEndsWithSeparator(void);
	bool ReadDirectoryEntries(void);
	bool ReadDirectorySpecificEntriesIntoStringList(std::list<cRZString>& list, uint32_t flags);

protected:
	std::list<cRZDirectoryEntry> dirEntries;
	uint32_t entryCount;
	cRZString path;
	cRZString entryFilter;
	bool includeCurrentDotDir;
	bool includeParentDotDir;
};