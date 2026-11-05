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

#pragma once
#include <vector>
#include "cIGZFile.h"
#include "cRZString.h"

class cRZFile : public cIGZFile
{
public:
	cRZFile(void);
	cRZFile(cIGZString const& pathStr);
	cRZFile(char const* pathCStr);
	virtual ~cRZFile(void);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual bool Open(uint32_t accessFlags, uint32_t securityFlags, uint32_t shareFlags);
	virtual bool IsOpen(void) const;
	virtual bool Close(void);

	virtual int32_t Position(void);
	virtual int32_t Length(void);
	virtual bool SetLength(uint32_t length);

	virtual int32_t SeekToBegin(void);
	virtual int32_t SeekToEnd(void);
	virtual int32_t SeekToRelativePosition(int32_t offset);
	virtual int32_t SeekToPosition(int32_t position);
	virtual int32_t Seek(int32_t offset, int32_t mode);

	virtual bool Read(void* data, uint32_t byteCount);
	virtual bool ReadWithCount(void* data, uint32_t& byteCount);

	virtual bool Write(void const* data, uint32_t byteCount);
	virtual bool WriteWithCount(void const* data, uint32_t& byteCount);
	virtual bool Flush(void);

	virtual bool Remove(void);
	virtual bool Rename(cIGZString const& newName);
	virtual bool Exists(void);
	virtual bool Copy(cIGZString const& srcName, bool overwrite);
	virtual bool Copy(cIGZFile const& srcFile, bool overwrite);

	virtual void GetPath(cIGZString& path) const;
	virtual cIGZString const& GetPath(void) const;
	virtual const char* GetPathCStr(void) const;
	virtual void SetPath(char const* path);
	virtual void SetPath(cIGZString const& path);

	virtual bool GetOpenModes(uint32_t& accessFlags, uint32_t& createMode, uint32_t& shareFlags);
	virtual void GetBuffering(uint32_t& readBufferSize, uint32_t& writeBufferSize);
	virtual bool SetBuffering(uint32_t newReadBufferSize, uint32_t newWriteBufferSize);

public:
	static bool GetTempDirectory(cIGZString& out);
	static bool MakeTempFileName(cIGZString const& prefix, cIGZString& name, cIGZString const* suffix);
	static bool MakeTempPathName(cIGZString& name, cIGZString const* suffix);
	static bool FileExists(cIGZString const& name);
	static bool Remove(cIGZString const& name);
	static bool Rename(cIGZString const& srcName, cIGZString const& destName);
	static bool Copy(cIGZString const& srcName, cIGZString const& destName, bool overwrite);
	static bool GetPathDirectory(cIGZString const& path, cIGZString& directory);
	static bool GetPathName(cIGZString const& path, cIGZString& name, bool includeSeparator);
	static bool GetVolumeName(cIGZString const& name, cIGZString& volume);

protected:
	void UpdateErrorCode(void);
	bool FillReadBuffer(void);
	bool FillWriteBuffer(uint8_t const* data, int size);
	bool FlushWriteBuffer(void);

protected:
	cRZString mPath;
	bool mOpen;
	void* mFileHandle;
	uint32_t mAccessFlags;
	uint32_t mCreateMode;
	uint32_t mShareFlags;
	uint32_t mRefCount;
	uint32_t mLastError;
	cRZString mScratch;
	uint32_t mPosition;
	uint32_t mFilePointer;
	uint32_t mReadBufferSize;
	std::vector<uint8_t> mReadBuffer;
	uint32_t mBufferedReadFilePosition;
	uint32_t mReadBytesBuffered;
	uint32_t mWriteBufferSize;
	std::vector<uint8_t> mWriteBuffer;
	uint32_t mBufferedWriteFilePosition;
	uint32_t mWriteBytesBuffered;
};