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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistDBSegmentDirectoryFiles = 0x856C8CF7;

struct cGZPersistResourceKey;
class cIGZPersistDBSegment;
class cIGZPersistRESIDtoExtensionMap;
class cIGZString;

class cIGZPersistDBSegmentDirectoryFiles : public cIGZUnknown
{
public:
	virtual cIGZPersistDBSegment* AsIDBSegment() = 0;

	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;

	virtual cIGZString const& Path() const = 0;
	virtual void GetPath(cIGZString& outPath) const = 0;
	virtual bool SetPath(cIGZString const& path) = 0;

	virtual void SetDefaultGroupId(uint32_t groupId) = 0;
	virtual uint32_t GroupId() const = 0;

	virtual cIGZPersistRESIDtoExtensionMap& ExtensionMap() = 0;
	virtual bool GetRESIDtoExtensionMap(cIGZPersistRESIDtoExtensionMap** out) = 0;
	virtual bool AddExtensionAssociation(uint32_t resid, cIGZString const& extension) = 0;

	virtual void SetAllowedIDPositions(bool allowGroup, bool allowInstance) = 0;
	virtual void SetAllowTextAsKey(bool allowTextAsKey) = 0;

	virtual bool GetFileNameFromKey(cGZPersistResourceKey const& key, cIGZString& fileName) const = 0;
	virtual bool GetFilePathFromKey(cGZPersistResourceKey const& key, cIGZString& filePath) const = 0;
	virtual bool GetKeyFromFileName(cIGZString const& fileName, cIGZString const& extension, cGZPersistResourceKey& key) const = 0;
	virtual bool IsFileNameValid(cIGZString const& fileName) const = 0;

	virtual bool GenerateNewKey(uint32_t type, cGZPersistResourceKey& key) = 0;
	virtual uint32_t EnumerateRecordFiles(cIGZString** names, uint32_t maxCount) = 0;

	virtual bool GetFileChangeDetection() const = 0;
	virtual bool SetFileChangeDetection(bool enabled, uint32_t intervalInSeconds) = 0;
	virtual bool DoFileChangeCheck() = 0;

	virtual bool Init(uint32_t segmentId, cIGZString const* path, bool checkDirectory) = 0;
};