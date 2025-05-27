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

// FUTURE: make platform agnostic
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <Windows.h>

#include "cRZFile.h"

cRZFile::cRZFile(void) :
	path(),
	isOpen(false),
	fileHandle(INVALID_HANDLE_VALUE),
	accessFlags(0),
	createMode(0),
	shareFlags(0),
	refCount(0),
	lastError(0x20000000),
	position(0),
	filePointer(0),
	readBufferSize(0),
	readBuffer(),
	bufferedReadFilePosition(0),
	readBytesBuffered(0),
	writeBufferSize(0),
	writeBuffer(),
	writeBytesBuffered(0)
{
}

cRZFile::cRZFile(cIGZString const& pathStr) :
	path(pathStr),
	isOpen(false),
	fileHandle(INVALID_HANDLE_VALUE),
	accessFlags(0),
	createMode(0),
	shareFlags(0),
	refCount(0),
	lastError(0x20000000),
	position(0),
	filePointer(0),
	readBufferSize(0),
	readBuffer(),
	bufferedReadFilePosition(0),
	readBytesBuffered(0),
	writeBufferSize(0),
	writeBuffer(),
	writeBytesBuffered(0)
{
}

cRZFile::cRZFile(char const* pathCStr) :
	path(),
	isOpen(false),
	fileHandle(INVALID_HANDLE_VALUE),
	accessFlags(0),
	createMode(0),
	shareFlags(0),
	refCount(0),
	lastError(0x20000000),
	position(0),
	filePointer(0),
	readBufferSize(0),
	readBuffer(),
	bufferedReadFilePosition(0),
	readBytesBuffered(0),
	writeBufferSize(0),
	writeBuffer(),
	writeBytesBuffered(0)
{
	if (pathCStr != NULL)
	{
		path = pathCStr;
	}
}

cRZFile::~cRZFile(void)
{
}

bool cRZFile::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZFile)
	{
		*outPtr = static_cast<cIGZFile*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cRZFile::AddRef(void)
{
	return ++refCount;
}

uint32_t cRZFile::Release(void)
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

bool cRZFile::Open(uint32_t accessFlags, uint32_t createMode, uint32_t shareFlags)
{
	if (isOpen)
	{
		return true;
	}

	DWORD fileAccessMode = 0;
	if (accessFlags & GZFileAccessRead)
	{
		fileAccessMode = GENERIC_READ;
	}

	if (accessFlags & GZFileAccessWrite)
	{
		fileAccessMode |= GENERIC_WRITE;
	}

	DWORD fileShareMode = 0;
	if (shareFlags != GZFileShareExclusive)
	{
		fileShareMode = FILE_SHARE_READ;
	}

	if (shareFlags & GZFileShareReadWrite)
	{
		fileShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

	DWORD fileDisposition = 0;
	switch (createMode)
	{
	case GZFileCreateNewOnly:
		fileDisposition = CREATE_NEW;
		break;

	case GZFileCreateNewOverwriteExisting:
		fileDisposition = CREATE_ALWAYS;
		break;

	case GZFileOpenExistingOnly:
		fileDisposition = OPEN_EXISTING;
		break;

	case GZFileCreateNewOpenExisting:
		fileDisposition = OPEN_ALWAYS;
		break;

	case GZFileOverwriteExistingOnly:
		fileDisposition = TRUNCATE_EXISTING;
		break;

	default:
		return false;
	}

	cRZString pathCopy(path);
	fileHandle = CreateFile(pathCopy.ToChar(), fileAccessMode, fileShareMode, NULL, fileDisposition, 0, NULL);
	
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		isOpen = true;

		this->accessFlags = accessFlags;
		this->createMode = createMode;
		this->shareFlags = shareFlags;

		DWORD offset = SetFilePointer(fileHandle, 0, 0, FILE_CURRENT);
		position = offset;
		filePointer = offset;

		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;
		bufferedWriteFilePosition = 0;
		writeBytesBuffered = 0;

		return true;
	}

	UpdateErrorCode();
	return false;
}

bool cRZFile::IsOpen(void) const
{
	return isOpen;
}

bool cRZFile::Close(void)
{
	if (!isOpen)
	{
		return false;
	}
	else
	{
		FlushWriteBuffer();

		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;
		bufferedWriteFilePosition = 0;
		writeBytesBuffered = 0;

		if (!CloseHandle(fileHandle))
		{
			UpdateErrorCode();
		}

		accessFlags = 0;
		createMode = 0;
		shareFlags = 0;
		filePointer = 0;

		fileHandle = INVALID_HANDLE_VALUE;
		isOpen = false;

		return true;
	}
}

int32_t cRZFile::Position(void)
{
	int ret = -1;
	if (isOpen && position != 1)
	{
		return position;
	}

	UpdateErrorCode();
	return -1;
}

int32_t cRZFile::Length(void)
{
	int ret = -1;
	if (isOpen)
	{
		ret = GetFileSize(fileHandle, NULL);
		if (ret == -1)
		{
			UpdateErrorCode();
		}
		else if (writeBytesBuffered != 0 && (uint32_t)ret < position)
		{
			ret = position;
		}
	}

	return ret;
}

bool cRZFile::SetLength(uint32_t length)
{
	if (isOpen)
	{
		bool result = false;

		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;
		FlushWriteBuffer();

		DWORD fileOffset = SetFilePointer(fileHandle, length, NULL, FILE_BEGIN);
		if (fileOffset == -1 || !SetEndOfFile(fileHandle))
		{
			UpdateErrorCode();
		}
		else
		{
			result = true;
		}

		DWORD newOffset = SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
		position = newOffset;
		filePointer = newOffset;

		return result;
	}

	return false;
}

int32_t cRZFile::SeekToBegin(void)
{
	return Seek(0, GZSeekFromBegin);
}

int32_t cRZFile::SeekToEnd(void)
{
	return Seek(0, GZSeekFromEnd);
}

int32_t cRZFile::SeekToRelativePosition(int32_t offset)
{
	return Seek(offset, GZSeekFromCurrent);
}

int32_t cRZFile::SeekToPosition(int32_t position)
{
	return Seek(position, GZSeekFromBegin);
}

int32_t cRZFile::Seek(int32_t offset, int32_t mode)
{
	if (!isOpen)
	{
		return -1;
	}

	if (mode != GZSeekFromBegin && mode != GZSeekFromEnd)
	{
		if (mode != GZSeekFromCurrent)
		{
			return Position();
		}

		int32_t offsetAdjustment = 0;
		bool isReadAligned = (offset == position);
		if (!isReadAligned)
		{
			offsetAdjustment = position;
		}

		offset += offsetAdjustment;
		mode = isReadAligned ? GZSeekFromCurrent : GZSeekFromBegin;
	}

	FlushWriteBuffer();
	DWORD newPosition = SetFilePointer(fileHandle, offset, NULL, mode);
	if (newPosition == -1)
	{
		DWORD recoveredPosition = SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
		position = recoveredPosition;
		filePointer = recoveredPosition;

		UpdateErrorCode();
	}
	else
	{
		position = newPosition;
		filePointer = newPosition;
	}

	return newPosition;
}

bool cRZFile::Read(void* data, uint32_t byteCount)
{
	uint32_t bytesRead = byteCount;
	return ReadWithCount(data, bytesRead) && bytesRead == byteCount;
}

bool cRZFile::ReadWithCount(void* data, uint32_t& byteCount)
{
	if (!isOpen)
	{
		return false;
	}
	else if (byteCount == 0)
	{
		return true;
	}

	if (writeBytesBuffered)
	{
		FlushWriteBuffer();
	}

	bool result = true;
	if (readBufferSize == 0)
	{
		DWORD bytesRead;
		if (!ReadFile(fileHandle, data, byteCount, &bytesRead, NULL))
		{
			filePointer = SetFilePointer(fileHandle, 0, 0, FILE_CURRENT);
			UpdateErrorCode();
			result = false;
		}
		else
		{
			filePointer += bytesRead;
		}

		byteCount = bytesRead;
		position = filePointer;
	}
	else
	{
		void* readTarget = data;
		uint32_t bytesToRead = byteCount;
		uint32_t cachedPosition = position;
		uint32_t cachedReadBufferStart = bufferedReadFilePosition;

		if (cachedReadBufferStart <= cachedPosition && cachedPosition < readBytesBuffered)
		{
			uint32_t n = readBytesBuffered - cachedPosition - cachedReadBufferStart;
			if (n > bytesToRead)
			{
				n = bytesToRead;
			}

			memcpy(data, &readBuffer[cachedPosition - cachedReadBufferStart], n);
			bytesToRead -= n;
			readTarget = reinterpret_cast<char*>(readTarget) + n;
			cachedPosition = position + n;
			position = cachedPosition;
		}

		result = true;
		while (bytesToRead != 0)
		{
			bufferedReadFilePosition = 0;
			readBytesBuffered = 0;

			if (filePointer != cachedPosition)
			{
				uint32_t newPosition = SeekToPosition(cachedPosition);
				result = (newPosition == position);
			}

			if (result)
			{
				result = FillReadBuffer();
			}

			if (!result)
			{
				break;
			}

			cachedReadBufferStart = readBytesBuffered;
			if (cachedReadBufferStart == 0)
			{
				break;
			}

			if (bytesToRead < cachedReadBufferStart)
			{
				cachedReadBufferStart = bytesToRead;
			}

			memcpy(readTarget, readBuffer.begin(), bytesToRead);
			readTarget = reinterpret_cast<char*>(readTarget) + bytesToRead;
			cachedPosition = position + cachedReadBufferStart;
			bytesToRead -= cachedReadBufferStart;
			position = cachedPosition;
		}

		byteCount -= bytesToRead;
	}

	return result;
}

bool cRZFile::Write(void const* data, uint32_t byteCount)
{
	uint32_t bytesWritten = byteCount;
	return WriteWithCount(data, bytesWritten) && bytesWritten == byteCount;
}

bool cRZFile::WriteWithCount(void const* data, uint32_t& byteCount)
{
	if (!isOpen)
	{
		return false;
	}

	if (readBytesBuffered != 0)
	{
		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;

		if (position != filePointer)
		{
			SeekToPosition(position);
		}
	}

	if (writeBufferSize == 0)
	{
		bool result;
		DWORD bytesWritten;

		result = WriteFile(fileHandle, data, byteCount, &bytesWritten, NULL);
		if (!result)
		{
			filePointer = SetFilePointer(fileHandle, 0, 0, FILE_CURRENT);
			UpdateErrorCode();
			bytesWritten = 0;
		}
		else
		{
			filePointer += bytesWritten;
		}

		byteCount = bytesWritten;
		position = filePointer;
		return result;
	}
	else
	{
		bool result = FillWriteBuffer(reinterpret_cast<uint8_t const*>(data), byteCount);
		position += byteCount;
		return result;
	}
}

bool cRZFile::Flush(void)
{
	bool result = isOpen;
	if (isOpen)
	{
		result = FlushWriteBuffer();
	}

	return result;
}

bool cRZFile::Remove(void)
{
	if (IsOpen())
	{
		Close();
	}

	cRZString pathCopy(path);
	if (!DeleteFile(pathCopy.ToChar()))
	{
		UpdateErrorCode();
		return false;
	}

	return true;
}

bool cRZFile::Rename(cIGZString const& newName)
{
	if (IsOpen())
	{
		Close();
	}

	cRZString previousPath(path);
	cRZString newPath(newName);

	bool succeeded = MoveFile(previousPath.ToChar(), newPath.ToChar());
	if (!succeeded)
	{
		UpdateErrorCode();
	}

	path = newPath;
	return succeeded;
}

bool cRZFile::Exists(void)
{
	cRZString pathCopy(path);
	DWORD attributes = GetFileAttributes(pathCopy.ToChar());

	if (attributes == -1)
	{
		UpdateErrorCode();
		return false;
	}

	return true;
}

bool cRZFile::Copy(cIGZFile const& srcFile, bool overwrite)
{
	cRZString srcPath(path);
	cRZString destPath(srcFile.GetPath());

	return Copy(srcPath, destPath, overwrite);
}

bool cRZFile::Copy(cIGZString const& srcName, bool overwrite)
{
	return Copy(srcName, path, overwrite);
}

void cRZFile::GetPath(cIGZString& path) const
{
	path.FromChar(this->path.ToChar(), this->path.Strlen());
}

cIGZString const& cRZFile::GetPath(void) const
{
	return path;
}

const char* cRZFile::GetPathCStr(void) const
{
	return path.ToChar();
}

void cRZFile::SetPath(cIGZString const& path)
{
	this->path = path;
}

void cRZFile::SetPath(char const* path)
{
	if (path != NULL)
	{
		this->path = cRZString(path);
	}
}

bool cRZFile::GetOpenModes(uint32_t& accessFlags, uint32_t& createMode, uint32_t& shareFlags)
{
	if (!isOpen)
	{
		return false;
	}

	accessFlags = this->accessFlags;
	createMode = this->createMode;
	shareFlags = this->shareFlags;
	return true;
}

void cRZFile::GetBuffering(uint32_t& readBufferSize, uint32_t& writeBufferSize)
{
	readBufferSize = this->readBufferSize;
	writeBufferSize = this->writeBufferSize;
}

bool cRZFile::SetBuffering(uint32_t newReadBufferSize, uint32_t newWriteBufferSize)
{
	const uint32_t kMinBufferSize = 4;
	const uint32_t kMaxBufferSize = 2048000;
	const uint32_t kDefaultBufferSize = 512;

	///////////////////////////////////
	// Reconfigure the read buffer
	///////////////////////////////////
	if (newReadBufferSize == -1)
	{
		newReadBufferSize = kDefaultBufferSize;
	}

	newReadBufferSize &= (~1);
	if (newReadBufferSize != 0 && newReadBufferSize < kMinBufferSize)
	{
		newReadBufferSize = kMinBufferSize;
	}

	if (newReadBufferSize > kMaxBufferSize)
	{
		newReadBufferSize = kMaxBufferSize;
	}

	if (newReadBufferSize < readBufferSize)
	{
		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;
	}

	readBufferSize = newReadBufferSize;
	if (!readBuffer.empty())
	{
		readBuffer.resize(newReadBufferSize);
	}

	///////////////////////////////////
	// Reconfigure the write buffer
	///////////////////////////////////
	if (newWriteBufferSize == -1)
	{
		newWriteBufferSize = kDefaultBufferSize;
	}

	newWriteBufferSize &= (~1);
	if (newWriteBufferSize != 0 && newWriteBufferSize < kMinBufferSize)
	{
		newWriteBufferSize = kMinBufferSize;
	}

	if (newWriteBufferSize > kMaxBufferSize)
	{
		newWriteBufferSize = kMaxBufferSize;
	}

	if (newWriteBufferSize < writeBufferSize)
	{
		FlushWriteBuffer();
	}

	writeBufferSize = newWriteBufferSize;
	if (!writeBuffer.empty())
	{
		writeBuffer.resize(newWriteBufferSize);
	}

	return true;
}

void cRZFile::UpdateErrorCode(void)
{
	DWORD result = GetLastError();
	if (result == ERROR_ACCESS_DENIED)
	{
		lastError = GZFileErrorAccessDenied;
		return;
	}

	if (result != ERROR_ALREADY_EXISTS)
	{
		lastError = result;
		return;
	}

	lastError = GZFileErrorAlreadyExists;
}

bool cRZFile::FillReadBuffer(void)
{
	if (readBuffer.empty())
	{
		readBuffer.resize(readBufferSize);
	}

	DWORD bytesRead;
	if (ReadFile(fileHandle, &readBuffer[0], readBufferSize, &bytesRead, NULL))
	{
		bufferedReadFilePosition = filePointer;
		filePointer += bytesRead;
		readBytesBuffered = bytesRead;

		return true;
	}
	else
	{
		filePointer = SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
		bufferedReadFilePosition = 0;
		readBytesBuffered = 0;

		return false;
	}
}

bool cRZFile::FillWriteBuffer(uint8_t const* data, int size)
{
	if (size <= 0)
	{
		return true;
	}

	if (writeBuffer.empty())
	{
		writeBuffer.resize(writeBufferSize);
	}

	if (writeBytesBuffered == 0)
	{
		bufferedWriteFilePosition = position;
	}

	if (writeBufferSize < writeBytesBuffered + size)
	{
		uint8_t const* readPointer = data;
		bool canWrite = true;

		while (size != 0 && canWrite)
		{
			uint32_t bytesToBuffer = writeBufferSize - writeBytesBuffered;
			if (size < bytesToBuffer)
			{
				bytesToBuffer = size;
			}

			if (bytesToBuffer != 0)
			{
				memcpy(&writeBuffer[writeBytesBuffered], readPointer, bytesToBuffer);
				writeBytesBuffered += bytesToBuffer;
				readPointer += bytesToBuffer;
				size -= bytesToBuffer;
			}

			if (writeBytesBuffered == writeBufferSize)
			{
				canWrite = FlushWriteBuffer();
			}
		}

		return canWrite;
	}
	else
	{
		memcpy(&readBuffer[writeBytesBuffered], data, size);
		writeBytesBuffered += size;
		return true;
	}
}

bool cRZFile::FlushWriteBuffer(void)
{
	if (writeBytesBuffered == 0)
	{
		return true;
	}

	DWORD bytesWritten;
	if (WriteFile(fileHandle, &writeBuffer[0], writeBytesBuffered, &bytesWritten, NULL))
	{
		uint32_t newFilePointer = filePointer + bytesWritten;
		filePointer = newFilePointer;
		bufferedWriteFilePosition = newFilePointer;
		writeBytesBuffered = 0;

		return true;
	}
	else
	{
		uint32_t newFilePointer = SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
		filePointer = newFilePointer;
		bufferedWriteFilePosition = newFilePointer;
		writeBytesBuffered = 0;

		return false;
	}
}

bool cRZFile::GetTempDirectory(cIGZString& out)
{
	char tmpPath[MAX_PATH];
	DWORD tmpPathLen = GetTempPath(MAX_PATH, tmpPath);

	if (tmpPathLen == 0)
	{
		return false;
	}

	// TODO: there is supposed to be a call to ConvertStringEncoding, which is
	// not implemented yet. We're just going to copy the string verbatim, which
	// probably does not work on Windows installations w/ non-Latin code pages.
	out = cRZString(tmpPath);
	return true;
}

bool cRZFile::MakeTempFileName(cIGZString const& prefix, cIGZString& name, cIGZString const* suffix)
{
	time_t seedNum = time(NULL);
	cRZString tmpName;
	cRZString originalName(name.Data(), name.Strlen());
	cRZString suffixStr;

	if (suffix == NULL)
	{
		suffixStr = cRZString(".tmp");
	}
	else
	{
		suffixStr = suffix;
	}

	if (originalName.Strlen() < 245)
	{
		if (originalName.Strlen() == 0)
		{
			originalName = cRZString("RZ_");
		}
	}
	else
	{
		originalName.Resize(245);
	}

	for (int i = 0; i < 10000; i++)
	{
		tmpName.Sprintf("%s%s%d%s", prefix.Data(), originalName, seedNum, suffixStr.Data());

		cRZFile newFile(tmpName);
		if (newFile.Open(GZFileAccessRead | GZFileAccessWrite, GZFileCreateNewOnly, GZFileShareExclusive))
		{
			name.FromChar(tmpName.Data(), tmpName.Strlen());
			return true;
		}
		else
		{
			seedNum--;
		}
	}

	return false;
}

bool cRZFile::MakeTempPathName(cIGZString& name, cIGZString const* suffix)
{
	time_t seedNum = time(NULL);
	cRZString tmpName;
	cRZString originalName(name.Data(), name.Strlen());

	cRZString tmpPath;
	GetTempDirectory(tmpPath);

	cRZString suffixStr;
	if (suffix == NULL)
	{
		suffixStr = cRZString(".tmp");
	}
	else
	{
		suffixStr = suffix;
	}

	if (originalName.Strlen() < 245)
	{
		if (originalName.Strlen() == 0)
		{
			originalName = cRZString("RZ_");
		}
	}
	else
	{
		originalName.Resize(245);
	}

	for (int i = 0; i < 10000; i++)
	{
		tmpName.Sprintf("%s%s%d%s", tmpPath.Data(), originalName, seedNum, suffixStr.Data());

		cRZFile newFile(tmpName);
		if (newFile.Open(GZFileAccessRead | GZFileAccessWrite, GZFileCreateNewOnly, GZFileShareExclusive))
		{
			name.FromChar(tmpName.Data(), tmpName.Strlen());
			return true;
		}
		else
		{
			seedNum--;
		}
	}

	return false;
}

bool cRZFile::FileExists(cIGZString const& name)
{
	// TODO: another missing ConvertStringEncoding call here and in Remove
	return GetFileAttributes(name.ToChar()) != -1;
}

bool cRZFile::Remove(cIGZString const& name)
{
	return DeleteFile(name.ToChar());
}

bool cRZFile::Copy(cIGZString const& srcName, cIGZString const& destName, bool overwrite)
{
	cRZString srcStr(srcName);
	cRZString destStr(destName);
	return CopyFile(srcStr.ToChar(), destStr.ToChar(), !overwrite);
}