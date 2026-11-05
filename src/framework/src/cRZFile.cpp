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

// FUTURE: make platform agnostic
#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <Windows.h>

#include "cRZFile.h"
#include "RZStringUtil.h"
#include "W32FileHelpers.h"

static const size_t kDefaultFileBufferSize = 512;

cRZFile::cRZFile(void) :
	mPath(),
	mOpen(false),
	mFileHandle(INVALID_HANDLE_VALUE),
	mAccessFlags(0),
	mCreateMode(0),
	mShareFlags(0),
	mRefCount(0),
	mLastError(0x20000000),
	mScratch(),
	mPosition(0),
	mFilePointer(0),
	mReadBufferSize(kDefaultFileBufferSize),
	mReadBuffer(),
	mBufferedReadFilePosition(0),
	mReadBytesBuffered(0),
	mWriteBufferSize(kDefaultFileBufferSize),
	mWriteBuffer(),
	mBufferedWriteFilePosition(0),
	mWriteBytesBuffered(0)
{
}

cRZFile::cRZFile(cIGZString const& pathStr) :
	mPath(pathStr),
	mOpen(false),
	mFileHandle(INVALID_HANDLE_VALUE),
	mAccessFlags(0),
	mCreateMode(0),
	mShareFlags(0),
	mRefCount(0),
	mLastError(0x20000000),
	mScratch(),
	mPosition(0),
	mFilePointer(0),
	mReadBufferSize(kDefaultFileBufferSize),
	mReadBuffer(),
	mBufferedReadFilePosition(0),
	mReadBytesBuffered(0),
	mWriteBufferSize(kDefaultFileBufferSize),
	mWriteBuffer(),
	mBufferedWriteFilePosition(0),
	mWriteBytesBuffered(0)
{
}

cRZFile::cRZFile(char const* pathCStr) :
	mPath(),
	mOpen(false),
	mFileHandle(INVALID_HANDLE_VALUE),
	mAccessFlags(0),
	mCreateMode(0),
	mShareFlags(0),
	mRefCount(0),
	mLastError(0x20000000),
	mScratch(),
	mPosition(0),
	mFilePointer(0),
	mReadBufferSize(kDefaultFileBufferSize),
	mReadBuffer(),
	mBufferedReadFilePosition(0),
	mReadBytesBuffered(0),
	mWriteBufferSize(kDefaultFileBufferSize),
	mWriteBuffer(),
	mBufferedWriteFilePosition(0),
	mWriteBytesBuffered(0)
{
	if (pathCStr != NULL)
	{
		mPath = pathCStr;
	}
}

cRZFile::~cRZFile(void)
{
	if (mOpen)
	{
		Close();
	}
}

bool cRZFile::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZFile)
	{
		*outPtr = static_cast<cIGZFile*>(this);
		AddRef();
		return true;
	}

	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZFile::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cRZFile::Release(void)
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cRZFile::Open(uint32_t mAccessFlags, uint32_t mCreateMode, uint32_t mShareFlags)
{
	if (mOpen)
	{
		return true;
	}

	DWORD fileAccessMode = 0;
	if (mAccessFlags & GZFileAccessRead)
	{
		fileAccessMode = GENERIC_READ;
	}

	if (mAccessFlags & GZFileAccessWrite)
	{
		fileAccessMode |= GENERIC_WRITE;
	}

	DWORD fileShareMode = 0;
	if (mShareFlags != GZFileShareExclusive)
	{
		fileShareMode = FILE_SHARE_READ;
	}

	if (mShareFlags & GZFileShareReadWrite)
	{
		fileShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

	DWORD fileDisposition = 0;
	switch (mCreateMode)
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

	cRZString pathCopy(mPath);
	mFileHandle = CreateFile(pathCopy.ToChar(), fileAccessMode, fileShareMode, NULL, fileDisposition, 0, NULL);
	
	if (mFileHandle != INVALID_HANDLE_VALUE)
	{
		mOpen = true;

		this->mAccessFlags = mAccessFlags;
		this->mCreateMode = mCreateMode;
		this->mShareFlags = mShareFlags;

		DWORD offset = SetFilePointer(mFileHandle, 0, 0, FILE_CURRENT);
		mPosition = offset;
		mFilePointer = offset;

		mBufferedReadFilePosition = 0;
		mReadBytesBuffered = 0;
		mBufferedWriteFilePosition = 0;
		mWriteBytesBuffered = 0;

		return true;
	}

	UpdateErrorCode();
	return false;
}

bool cRZFile::IsOpen(void) const
{
	return mOpen;
}

bool cRZFile::Close(void)
{
	if (mOpen)
	{
		FlushWriteBuffer();

		mBufferedReadFilePosition = 0;
		mReadBytesBuffered = 0;
		mBufferedWriteFilePosition = 0;
		mWriteBytesBuffered = 0;

		if (!CloseHandle(mFileHandle))
		{
			UpdateErrorCode();
		}

		mAccessFlags = 0;
		mCreateMode = 0;
		mShareFlags = 0;
		mFilePointer = 0;

		mFileHandle = INVALID_HANDLE_VALUE;
		mOpen = false;

		return true;
	}

	return false;
}

int32_t cRZFile::Position(void)
{
	int32_t result = -1;
	if (mOpen)
	{
		result = mPosition;
	}

	return result;
}

int32_t cRZFile::Length(void)
{
	int ret = -1;
	if (mOpen)
	{
		ret = GetFileSize(mFileHandle, NULL);
		if (ret == -1)
		{
			UpdateErrorCode();
		}
		else if (mWriteBytesBuffered != 0 && (uint32_t)ret < mPosition)
		{
			ret = mPosition;
		}
	}

	return ret;
}

bool cRZFile::SetLength(uint32_t length)
{
	bool result = false;

	if (mOpen)
	{
		mBufferedReadFilePosition = 0;
		mReadBytesBuffered = 0;
		FlushWriteBuffer();

		DWORD fileOffset = SetFilePointer(mFileHandle, length, NULL, FILE_BEGIN);
		if (fileOffset == -1)
		{
			UpdateErrorCode();
			result = false;
		}
		else if (!SetEndOfFile(mFileHandle))
		{
			UpdateErrorCode();
			result = false;
		}
		else
		{
			result = true;
		}

		DWORD newOffset = SetFilePointer(mFileHandle, 0, NULL, FILE_CURRENT);
		mFilePointer = newOffset;
		mPosition = newOffset;
	}

	return result;
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

int32_t cRZFile::SeekToPosition(int32_t mPosition)
{
	return Seek(mPosition, GZSeekFromBegin);
}

int32_t cRZFile::Seek(int32_t offset, int32_t mode)
{
	if (mOpen)
	{
		int moveMethod;
		switch (mode)
		{
		case GZSeekFromBegin:
			moveMethod = FILE_BEGIN;
			break;

		case GZSeekFromCurrent:
			moveMethod = FILE_CURRENT;
			if (mFilePointer != mPosition)
			{
				moveMethod = FILE_BEGIN;
				offset += mPosition;
			}

			break;

		case GZSeekFromEnd:
			moveMethod = FILE_END;
			break;

		default:
			return Position();
		}

		FlushWriteBuffer();

		DWORD newPosition = SetFilePointer(mFileHandle, offset, NULL, moveMethod);
		if (newPosition == -1)
		{
			DWORD currentPosition = SetFilePointer(mFileHandle, 0, NULL, FILE_CURRENT);
			mFilePointer = currentPosition;
			mPosition = currentPosition;
			UpdateErrorCode();
		}
		else
		{
			mFilePointer = newPosition;
			mPosition = newPosition;
		}

		return (int32_t)newPosition;
	}

	return -1;
}

bool cRZFile::Read(void* data, uint32_t byteCount)
{
	uint32_t bytesRead = byteCount;

	bool result = ReadWithCount(data, bytesRead);
	if (result)
	{
		return byteCount == bytesRead;
	}

	return false;
}

bool cRZFile::ReadWithCount(void* data, uint32_t& byteCount)
{
	if (mOpen)
	{
		if (byteCount != 0)
		{
			if (mWriteBytesBuffered != 0)
			{
				FlushWriteBuffer();
			}

			if (mReadBufferSize != 0)
			{
				uint32_t bytesToRead = byteCount;
				void* readTarget = data;
				bool result = true;

				if (mPosition >= mBufferedReadFilePosition && mPosition < (mReadBytesBuffered + mBufferedReadFilePosition))
				{
					uint32_t const& bytesBuffered = mReadBytesBuffered;
					uint32_t offset = mPosition - mBufferedReadFilePosition;
					uint32_t n = bytesBuffered - offset;

					if (n >= bytesToRead)
					{
						n = bytesToRead;
					}

					memcpy(data, &mReadBuffer[offset], n);

					readTarget = reinterpret_cast<uint8_t*>(data) + n;
					bytesToRead -= n;
					mPosition += n;
				}

				while (bytesToRead != 0)
				{
					mBufferedReadFilePosition = 0;
					mReadBytesBuffered = 0;

					if (mFilePointer != mPosition)
					{
						uint32_t newPosition = SeekToPosition(mPosition);
						result = (newPosition == mPosition);
					}

					if (result)
					{
						result = FillReadBuffer();
					}

					if (!result)
					{
						break;
					}

					uint32_t bytesAvailable = mReadBytesBuffered;
					if (bytesAvailable == 0)
					{
						break;
					}

					uint32_t n = bytesAvailable;
					if (n >= bytesToRead)
					{
						n = bytesToRead;
					}

					memcpy(readTarget, &mReadBuffer[0], n);

					readTarget = reinterpret_cast<uint8_t*>(data) + n;
					bytesToRead -= n;
					mPosition += n;
				}
			}
			else
			{
				BOOL result = ReadFile(mFileHandle, data, byteCount, reinterpret_cast<LPDWORD>(&byteCount), NULL);
				if (result)
				{
					mFilePointer += byteCount;
				}
				else
				{
					mFilePointer = SetFilePointer(mFileHandle, 0, NULL, FILE_CURRENT);
					UpdateErrorCode();
				}

				mPosition = mFilePointer;
				return result != FALSE;
			}
		}

		return true;
	}

	return false;
}

bool cRZFile::Write(void const* data, uint32_t byteCount)
{
	uint32_t bytesWritten = byteCount;

	bool result = WriteWithCount(data, bytesWritten);
	if (result)
	{
		return byteCount == bytesWritten;
	}

	return false;
}

bool cRZFile::WriteWithCount(void const* data, uint32_t& byteCount)
{
	if (mOpen)
	{
		if (mReadBytesBuffered != 0)
		{
			uint32_t savedPosition = mPosition;
			mBufferedReadFilePosition = 0;
			mReadBytesBuffered = 0;

			if (savedPosition != mFilePointer)
			{
				SeekToPosition(savedPosition);
			}
		}

		if (mWriteBufferSize != 0)
		{
			bool result = FillWriteBuffer(reinterpret_cast<uint8_t const*>(data), byteCount);
			mPosition += byteCount;
			return result;
		}

		BOOL result = WriteFile(mFileHandle, data, byteCount, reinterpret_cast<LPDWORD>(&byteCount), NULL);
		if (result)
		{
			mFilePointer += byteCount;
		}
		else
		{
			mFilePointer = SetFilePointer(mFileHandle, 0, 0, FILE_CURRENT);
			UpdateErrorCode();
		}

		mPosition = mFilePointer;
		return result != 0;
	}

	return false;
}

bool cRZFile::Flush(void)
{
	if (mOpen)
	{
		bool needFlush = (mWriteBytesBuffered > 0);
		FlushWriteBuffer();

		bool result = true;
		if (needFlush)
		{
			result = FlushFileBuffers(mFileHandle);
			if (!result)
			{
				UpdateErrorCode();
			}
		}

		return result;
	}

	return false;
}

bool cRZFile::Remove(void)
{
	if (IsOpen())
	{
		Close();
	}

	if (gW32FileHelpers.DeleteFileW != NULL)
	{
		BOOL result = gW32FileHelpers.DeleteFileW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(mPath)[0]));
		if (result == FALSE)
		{
			UpdateErrorCode();
		}

		return result != FALSE;
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(mPath, str, GZ_CODEPAGE_SYSTEM);

		BOOL result = DeleteFile(str.ToChar());
		if (result == FALSE)
		{
			UpdateErrorCode();
		}

		return result != FALSE;
	}
}

bool cRZFile::Rename(cIGZString const& newName)
{
	if (IsOpen())
	{
		Close();
	}

	bool succeeded = Rename(mPath, newName);
	if (!succeeded)
	{
		UpdateErrorCode();
	}

	mPath = newName;
	return succeeded;
}

bool cRZFile::Exists(void)
{
	bool succeeded = FileExists(mPath);
	if (!succeeded)
	{
		UpdateErrorCode();
	}

	return succeeded;
}

bool cRZFile::Copy(cIGZFile const& srcFile, bool overwrite)
{
	cRZString srcPath(srcFile.GetPath());

	bool succeeded = Copy(srcPath, mPath, overwrite);
	if (!succeeded)
	{
		UpdateErrorCode();
	}

	return succeeded;
}

bool cRZFile::Copy(cIGZString const& srcName, bool overwrite)
{
	bool succeeded = Copy(srcName, mPath, overwrite);
	if (!succeeded)
	{
		UpdateErrorCode();
	}

	return succeeded;
}

void cRZFile::GetPath(cIGZString& mPath) const
{
	mPath.FromChar(this->mPath.begin(), this->mPath.size());
}

cIGZString const& cRZFile::GetPath(void) const
{
	return mPath;
}

const char* cRZFile::GetPathCStr(void) const
{
	return mPath.c_str();
}

void cRZFile::SetPath(cIGZString const& path)
{
	uint32_t const len = path.Strlen();
	char const* data = path.Data();

	mPath.assign(data, data + len);
}

void cRZFile::SetPath(char const* path)
{
	if (path != NULL)
	{
		cRZString newPath(path);
		mPath = newPath;
	}
}

bool cRZFile::GetOpenModes(uint32_t& accessFlags, uint32_t& createMode, uint32_t& shareFlags)
{
	if (mOpen)
	{
		accessFlags = mAccessFlags;
		createMode = mCreateMode;
		shareFlags = mShareFlags;
		return true;
	}

	return false;
}

void cRZFile::GetBuffering(uint32_t& mReadBufferSize, uint32_t& mWriteBufferSize)
{
	mReadBufferSize = this->mReadBufferSize;
	mWriteBufferSize = this->mWriteBufferSize;
}

bool cRZFile::SetBuffering(uint32_t newReadBufferSize, uint32_t newWriteBufferSize)
{
	const uint32_t kMinBufferSize = 4;
	const uint32_t kMaxBufferSize = 2048000;

	///////////////////////////////////
	// Reconfigure the read buffer
	///////////////////////////////////
	if (newReadBufferSize == -1)
	{
		newReadBufferSize = kDefaultFileBufferSize;
	}

	newReadBufferSize &= (~1);
	if (newReadBufferSize > 0)
	{
		if (newReadBufferSize < kMinBufferSize)
		{
			newReadBufferSize = kMinBufferSize;
		}
	}

	if (newReadBufferSize > kMaxBufferSize)
	{
		newReadBufferSize = kMaxBufferSize;
	}

	if (newReadBufferSize < mReadBufferSize)
	{
		mBufferedReadFilePosition = 0;
		mReadBytesBuffered = 0;
	}

	mReadBufferSize = newReadBufferSize;
	if (!mReadBuffer.empty())
	{
		mReadBuffer.resize(newReadBufferSize);
	}

	///////////////////////////////////
	// Reconfigure the write buffer
	///////////////////////////////////
	if (newWriteBufferSize == -1)
	{
		newWriteBufferSize = kDefaultFileBufferSize;
	}

	newWriteBufferSize &= (~1);
	if (newWriteBufferSize > 0)
	{
		if (newWriteBufferSize < kMinBufferSize)
		{
			newWriteBufferSize = kMinBufferSize;
		}
	}

	if (newWriteBufferSize > kMaxBufferSize)
	{
		newWriteBufferSize = kMaxBufferSize;
	}

	if (newWriteBufferSize < mWriteBufferSize)
	{
		FlushWriteBuffer();
	}

	mWriteBufferSize = newWriteBufferSize;
	if (!mWriteBuffer.empty())
	{
		mWriteBuffer.resize(newWriteBufferSize);
	}

	return true;
}

void cRZFile::UpdateErrorCode(void)
{
	DWORD result = GetLastError();
	switch (result)
	{
	case ERROR_ACCESS_DENIED:
		mLastError = GZFileErrorAccessDenied;
		break;

	case ERROR_ALREADY_EXISTS:
		mLastError = result;
		break;

	default:
		mLastError = result;
		break;
	}
}

bool cRZFile::FillReadBuffer(void)
{
	bool result = false;

	if (mReadBuffer.empty())
	{
		mReadBuffer.resize(mReadBufferSize);
	}

	DWORD bytesRead = mReadBufferSize;;
	if (ReadFile(mFileHandle, &mReadBuffer[0], mReadBufferSize, &bytesRead, NULL))
	{
		mBufferedReadFilePosition = mFilePointer;
		mFilePointer += bytesRead;
		mReadBytesBuffered = bytesRead;
		result = true;
	}
	else
	{
		mFilePointer = SetFilePointer(mFileHandle, 0, NULL, FILE_CURRENT);
		mBufferedReadFilePosition = 0;
		mReadBytesBuffered = 0;
	}

	return result;
}

bool cRZFile::FillWriteBuffer(uint8_t const* data, int size)
{
	bool result = true;
	if (size > 0)
	{
		if (mWriteBuffer.empty())
		{
			mWriteBuffer.resize(mWriteBufferSize);
		}

		if (mWriteBytesBuffered == 0)
		{
			mBufferedWriteFilePosition = mFilePointer;
		}

		if ((int)mWriteBytesBuffered + size <= (int)mWriteBufferSize)
		{
			memcpy(&mWriteBuffer[mWriteBytesBuffered], data, size);
			mWriteBytesBuffered += size;
		}
		else
		{
			do
			{
				if (!result)
				{
					break;
				}

				int bytesToBuffer = (int)(mWriteBufferSize - mWriteBytesBuffered);
				if (bytesToBuffer >= size)
				{
					bytesToBuffer = size;
				}

				if (bytesToBuffer != 0)
				{
					memcpy(&mWriteBuffer[mWriteBytesBuffered], data, bytesToBuffer);
					mWriteBytesBuffered += bytesToBuffer;
					data += bytesToBuffer;
					size -= bytesToBuffer;
				}

				if (mWriteBytesBuffered == mWriteBufferSize)
				{
					result = FlushWriteBuffer();
				}
			}
			while (size != 0);
		}
	}

	return result;
}

bool cRZFile::FlushWriteBuffer(void)
{
	bool result = true;
	if (mWriteBytesBuffered != 0)
	{
		DWORD count = mWriteBytesBuffered;
		if (WriteFile(mFileHandle, mWriteBuffer.begin(), count, &count, NULL))
		{
			mFilePointer += count;
			mBufferedWriteFilePosition = mFilePointer;
		}
		else
		{
			mFilePointer = SetFilePointer(mFileHandle, 0, NULL, FILE_CURRENT);
			result = false;
			mBufferedWriteFilePosition = mFilePointer;
		}

		mWriteBytesBuffered = 0;
	}

	return result;
}

bool cRZFile::GetTempDirectory(cIGZString& out)
{
	if (gW32FileHelpers.GetTempPathW != NULL && gW32FileHelpers.GetLongPathNameW != NULL)
	{
		wchar_t widePath[MAX_PATH + 1];
		DWORD length = gW32FileHelpers.GetTempPathW(MAX_PATH, widePath);

		if (length == 0)
		{
			return false;
		}

		if (gW32FileHelpers.GetLongPathNameW != NULL)
		{
			length = gW32FileHelpers.GetLongPathNameW(widePath, widePath, MAX_PATH);
		}

		ConvertStringEncoding(widePath, length, GZ_CODEPAGE_UTF16, out);
		return true;
	}

	char ansiPath[MAX_PATH + 1];
	DWORD tmpPathLen = GetTempPath(MAX_PATH, ansiPath);

	if (tmpPathLen != 0)
	{
		if (gW32FileHelpers.GetLongPathNameA != NULL)
		{
			tmpPathLen = gW32FileHelpers.GetLongPathNameA(ansiPath, ansiPath, MAX_PATH);
		}

		cRZString str(ansiPath, tmpPathLen);
		ConvertStringEncoding(str, GZ_CODEPAGE_SYSTEM, out);

		return true;
	}

	return false;
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
	if (gW32FileHelpers.GetFileAttributesW != NULL)
	{
		bool result = gW32FileHelpers.GetFileAttributesW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(name)[0])) != -1;
		return result;
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(name, str, GZ_CODEPAGE_SYSTEM);

		bool result = GetFileAttributes(str.Data()) != -1;
		return result;
	}
}

bool cRZFile::Remove(cIGZString const& name)
{
	if (gW32FileHelpers.DeleteFileW != NULL)
	{
		bool result = gW32FileHelpers.DeleteFileW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(name)[0])) != FALSE;
		return result;
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(name, str, GZ_CODEPAGE_SYSTEM);

		bool result = DeleteFile(str.Data()) != 0;
		return result;
	}
}

bool cRZFile::Rename(cIGZString const& srcName, cIGZString const& destName)
{
	if (gW32FileHelpers.MoveFileExW != NULL)
	{
		bool result = gW32FileHelpers.MoveFileExW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(srcName)[0]),
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(destName)[0]),
			MOVEFILE_COPY_ALLOWED) != 0;

		return result;
	}
	else
	{
		cRZString srcStr;
		cRZString destStr;

		ConvertStringEncoding(srcName, srcStr, GZ_CODEPAGE_SYSTEM);
		ConvertStringEncoding(destName, destStr, GZ_CODEPAGE_SYSTEM);

		bool result = MoveFile(srcStr.Data(), destStr.Data()) != 0;
		return result;
	}
}

bool cRZFile::Copy(cIGZString const& srcName, cIGZString const& destName, bool overwrite)
{
	if (gW32FileHelpers.CopyFileW != NULL)
	{
		bool result = gW32FileHelpers.CopyFileW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(srcName)[0]),
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(destName)[0]),
			!overwrite) != 0;

		return result;
	}
	else
	{
		cRZString srcStr;
		cRZString destStr;

		ConvertStringEncoding(srcName, srcStr, GZ_CODEPAGE_SYSTEM);
		ConvertStringEncoding(destName, destStr, GZ_CODEPAGE_SYSTEM);

		bool result = CopyFile(srcStr.Data(), destStr.Data(), !overwrite) != 0;
		return result;
	}
}

bool cRZFile::GetPathDirectory(cIGZString const& name, cIGZString& out)
{
	char drive[_MAX_DRIVE + 1] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	_splitpath(name.ToChar(), drive, dir, fname, ext);
	size_t driveLength = strlen(drive);

	if (driveLength != 0)
	{
		drive[2] = '\\';
		drive[3] = '\0';
	}

	return driveLength != 0;
}

bool cRZFile::GetPathName(cIGZString const& name, cIGZString& out, bool includeSeparator)
{
	char drive[_MAX_DRIVE] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	cRZString path;
	_splitpath(name.ToChar(), drive, dir, fname, ext);

	path = cRZString(fname);
	if (includeSeparator)
	{
		path += ext;
	}

	out.FromChar(path.c_str(), path.size());
	return path.size() != 0;
}

bool cRZFile::GetVolumeName(cIGZString const& name, cIGZString& out)
{
	cRZString dir;
	if (!GetPathDirectory(name, dir))
	{
		return false;
	}

	char volumeName[MAX_PATH + 1];
	char fileSystemName[MAX_PATH + 1];
	DWORD serialNumber, maxComponentLength, flags;

	if (GetVolumeInformation(dir.ToChar(), volumeName, MAX_PATH, &serialNumber, &maxComponentLength, &flags, fileSystemName, MAX_PATH) != 0)
	{
		cRZString volume(volumeName);
		ConvertStringEncoding(volume, GZ_CODEPAGE_SYSTEM, out);
		return true;
	}

	return false;
}