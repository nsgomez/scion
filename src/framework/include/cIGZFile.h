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

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZFile = 0xE0203645;

class cIGZString;

enum GZFileError
{
	GZFileErrorSuccess = 0x20000000,
	GZFileErrorAccessDenied,
	GZFileErrorAlreadyExists,
};

enum GZFileAccessFlags
{
	GZFileAccessRead = 1,
	GZFileAccessWrite = 2,
	GZFileAccessReadWrite = GZFileAccessRead | GZFileAccessWrite,
};

enum GZFileCreateMode
{
	GZFileCreateNewOnly = 0,
	GZFileCreateNewOverwriteExisting,
	GZFileOpenExistingOnly,
	GZFileCreateNewOpenExisting,
	GZFileOverwriteExistingOnly,
};

enum GZFileShareFlags
{
	GZFileShareExclusive = 0,
	GZFileShareRead = 1,
	GZFileShareReadWrite = 2,
};

enum GZFileSeekMode
{
	GZSeekFromBegin = 0,
	GZSeekFromCurrent,
	GZSeekFromEnd,
};

class cIGZFile : public cIGZUnknown
{
public:
	virtual bool Open(uint32_t accessFlags, uint32_t securityFlags, uint32_t shareFlags) = 0;
	virtual bool IsOpen(void) const = 0;
	virtual bool Close(void) = 0;

	virtual int32_t Position(void) = 0;
	virtual int32_t Length(void) = 0;
	virtual bool SetLength(uint32_t length) = 0;

	virtual int32_t SeekToBegin(void) = 0;
	virtual int32_t SeekToEnd(void) = 0;
	virtual int32_t SeekToRelativePosition(int32_t offset) = 0;
	virtual int32_t SeekToPosition(int32_t position) = 0;
	virtual int32_t Seek(int32_t offset, int32_t mode) = 0;

	virtual bool Read(void* data, uint32_t byteCount) = 0;
	virtual bool ReadWithCount(void* data, uint32_t& byteCount) = 0;

	virtual bool Write(void const* data, uint32_t byteCount) = 0;
	virtual bool WriteWithCount(void const* data, uint32_t& byteCount) = 0;
	virtual bool Flush(void) = 0;

	virtual bool Remove(void) = 0;
	virtual bool Rename(cIGZString const& newName) = 0;
	virtual bool Exists(void) = 0;
	virtual bool Copy(cIGZFile const& srcFile, bool overwrite) = 0;
	virtual bool Copy(cIGZString const& srcName, bool overwrite) = 0;

	virtual void GetPath(cIGZString& path) const = 0;
	virtual cIGZString const& GetPath(void) const = 0;
	virtual const char* GetPathCStr(void) const = 0;
	virtual void SetPath(cIGZString const& path) = 0;
	virtual void SetPath(char const* path) = 0;

	virtual bool GetOpenModes(uint32_t& accessFlags, uint32_t& createMode, uint32_t& shareFlags) = 0;
	virtual void GetBuffering(uint32_t& readBufferSize, uint32_t& writeBufferSize) = 0;
	virtual bool SetBuffering(uint32_t newReadBufferSize, uint32_t newWriteBufferSize) = 0;
};