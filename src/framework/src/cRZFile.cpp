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

bool cRZFile::Open(uint32_t accessFlags, uint32_t createMode, uint32_t shareFlags)
{
	if (IsOpen())
	{
		return true;
	}

	DWORD fileDisposition = 0;
	DWORD fileAccessMode = 0;
	DWORD fileShareMode = 0;

	if (mAccessFlags & GZFileAccessRead)
	{
		fileAccessMode = GENERIC_READ;
	}

	if (mAccessFlags & GZFileAccessWrite)
	{
		fileAccessMode |= GENERIC_WRITE;
	}

	if (mShareFlags != GZFileShareExclusive)
	{
		fileShareMode = FILE_SHARE_READ;
	}

	if (mShareFlags & GZFileShareReadWrite)
	{
		fileShareMode |= FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

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

	if (gW32FileHelpers.CreateFileW != NULL)
	{
		std::vector<wchar_t> widePath = RZStringToUTF16(mPath);
		std::vector<wchar_t> resolvedPath;

		if (ResolveShellLinkW(widePath, resolvedPath))
		{
			widePath.swap(resolvedPath);
		}

		mFileHandle = gW32FileHelpers.CreateFileW(&widePath[0], fileAccessMode, fileShareMode, NULL, fileDisposition, 0, NULL);
	}
	else
	{
		cRZString path;
		ConvertStringEncoding(mPath, path, kGZCodePageSystem);

		char resolvedPath[MAX_PATH];
		DWORD resolvedPathLen = ResolveShellLinkA(path.c_str(), resolvedPath, MAX_PATH);

		if (resolvedPathLen > 0 && resolvedPathLen != 0xFFFFFFFF)
		{
			mFileHandle = CreateFile(resolvedPath, fileAccessMode, fileShareMode, NULL, fileDisposition, 0, NULL);
		}
		else
		{
			mFileHandle = CreateFile(path.c_str(), fileAccessMode, fileShareMode, NULL, fileDisposition, 0, NULL);
		}
	}

	if (mFileHandle == INVALID_HANDLE_VALUE)
	{
		UpdateErrorCode();
		return false;
	}

	mOpen = true;

	mAccessFlags = accessFlags;
	mCreateMode = createMode;
	mShareFlags = shareFlags;

	DWORD offset = SetFilePointer(mFileHandle, 0, 0, FILE_CURRENT);
	mPosition = offset;
	mFilePointer = offset;

	mBufferedReadFilePosition = 0;
	mReadBytesBuffered = 0;
	mBufferedWriteFilePosition = 0;
	mWriteBytesBuffered = 0;

	return true;
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
				bool result = true;
				void* readTarget = data;
				int32_t bytesToRead = (int32_t)byteCount;

				if ((int32_t)mPosition >= (int32_t)mBufferedReadFilePosition
					&& (int32_t)mPosition < (int32_t)mReadBytesBuffered + (int32_t)mBufferedReadFilePosition)
				{
					int32_t offset = (int32_t)(mPosition - mBufferedReadFilePosition);
					int32_t n = (int32_t)mReadBytesBuffered - offset;

					if (n >= bytesToRead)
					{
						n = bytesToRead;
					}

					memmove(data, &mReadBuffer[offset], n);

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

					if (mReadBytesBuffered == 0)
					{
						break;
					}

					int32_t n = (int32_t)mReadBytesBuffered;
					if (n >= bytesToRead)
					{
						n = bytesToRead;
					}

					memmove(readTarget, &mReadBuffer[0], n);

					readTarget = reinterpret_cast<uint8_t*>(readTarget) + n;
					bytesToRead -= n;
					mPosition += n;
				}

				byteCount -= bytesToRead;
				return result;
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

		BOOL result = TRUE;
		if (needFlush)
		{
			result = FlushFileBuffers(mFileHandle);
			if (result == FALSE)
			{
				UpdateErrorCode();
			}
		}

		return result != FALSE;
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
		ConvertStringEncoding(mPath, str, kGZCodePageSystem);

		BOOL result = DeleteFile(str.c_str());
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
	bool succeeded = Copy(cRZString(srcFile.GetPath()), mPath, overwrite);
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
		mLastError = GZFileErrorAlreadyExists;
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
			memmove(&mWriteBuffer[mWriteBytesBuffered], data, size);
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
					memmove(&mWriteBuffer[mWriteBytesBuffered], data, bytesToBuffer);
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

		ConvertStringEncoding(widePath, length, kGZCodePageUTF16, out);
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
		ConvertStringEncoding(str, kGZCodePageSystem, out);

		return true;
	}

	return false;
}

bool cRZFile::MakeTempFileName(cIGZString const& prefix, cIGZString& name, cIGZString const* suffix)
{
	static uint32_t kMaxTempNameLength = 244;

	int32_t seedNum = (int32_t)time(NULL);
	cRZString tmpName;
	cRZString originalName(name);
	cRZString suffixStr;

	if (suffix != NULL)
	{
		suffixStr = suffix;
	}
	else
	{
		suffixStr = cRZString(".tmp");
	}

	LimitStringLength(originalName, kMaxTempNameLength);
	if (originalName.empty())
	{
		originalName = cRZString("RZ_");
	}

	for (int32_t i = 0; i < 10000; i++)
	{
		cRZString tmp; // TODO: unused?
		tmpName.Sprintf("%s%s%d%s", prefix.ToChar(), originalName.begin(), seedNum, suffixStr.begin());

		cRZFile newFile(tmpName);
		if (newFile.Open(GZFileAccessReadWrite, GZFileCreateNewOnly, GZFileShareExclusive))
		{
			newFile.Close();
			name.FromChar(tmpName.begin(), tmpName.size());
			return true;
		}

		seedNum--;
	}

	return false;
}

bool cRZFile::FileCreate(cIGZString const& path, bool truncate)
{
	SECURITY_ATTRIBUTES securityAttributes;
	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttributes.lpSecurityDescriptor = NULL;
	securityAttributes.bInheritHandle = FALSE;

	HANDLE hFile;
	if (gW32FileHelpers.CreateFileW != NULL)
	{
		hFile = gW32FileHelpers.CreateFileW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0]),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			&securityAttributes,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(path, str, kGZCodePageSystem);

		hFile = CreateFile(
			str.Data(),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			&securityAttributes,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}

	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (truncate)
		{
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
		}

		CloseHandle(hFile);
		return true;
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

bool cRZFile::FileExists(cIGZString const& path)
{
	if (gW32FileHelpers.GetFileAttributesW != NULL)
	{
		bool result = gW32FileHelpers.GetFileAttributesW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0])) != -1;
		return result;
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(path, str, kGZCodePageSystem);

		bool result = GetFileAttributes(str.Data()) != -1;
		return result;
	}
}

bool cRZFile::GetAttribute(cIGZString const& path, uint32_t attributesToCheck)
{
	uint32_t attributes;
	if (gW32FileHelpers.GetFileAttributesW != NULL)
	{
		attributes = gW32FileHelpers.GetFileAttributesW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0]));
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(path, str, kGZCodePageSystem);

		attributes = GetFileAttributes(str.c_str());
	}

	if (((attributesToCheck & RZFileAttributeWritable) == 0 || (attributes & FILE_ATTRIBUTE_READONLY) == 0)
		|| ((attributesToCheck & RZFileAttributeDirectory) == 0 || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
	{
		if (((attributesToCheck & RZFileAttributeArchive) == 0 || (attributes & FILE_ATTRIBUTE_ARCHIVE) != 0)
			&& ((attributesToCheck & RZFileAttributeHidden) == 0 || (attributes & FILE_ATTRIBUTE_HIDDEN) != 0)
			&& ((attributesToCheck & RZFileAttributeSystem) == 0 || (attributes & FILE_ATTRIBUTE_SYSTEM) != 0)
			&& ((attributesToCheck & RZFileAttributeReadOnly) == 0 || (attributes & FILE_ATTRIBUTE_READONLY) != 0))
		{
			return true;
		}
	}

	return false;
}

bool cRZFile::SetAttribute(cIGZString const& path, uint32_t attributesToSet, bool value)
{
	uint32_t attributes;
	bool result;
	cRZString str;

	if (gW32FileHelpers.GetFileAttributesW == NULL || gW32FileHelpers.SetFileAttributesW == NULL)
	{
		ConvertStringEncoding(path, str, kGZCodePageSystem);
		attributes = GetFileAttributes(str.c_str());
	}
	else
	{
		attributes = gW32FileHelpers.GetFileAttributesW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0]));
	}

	uint32_t newAttributes = attributes;
	if ((attributesToSet & RZFileAttributeWritable) != 0)
	{
		newAttributes = value ? (newAttributes & ~FILE_ATTRIBUTE_READONLY) : (newAttributes | FILE_ATTRIBUTE_READONLY);
	}

	if ((attributesToSet & RZFileAttributeDirectory) != 0 || (attributesToSet & RZFileAttributeSymbolicLink) != 0)
	{
		return false;
	}

	if ((attributesToSet & RZFileAttributeArchive) != 0)
	{
		newAttributes = value ? (newAttributes | FILE_ATTRIBUTE_ARCHIVE) : (newAttributes & ~FILE_ATTRIBUTE_ARCHIVE);
	}

	if ((attributesToSet & RZFileAttributeHidden) != 0)
	{
		newAttributes = value ? (newAttributes | FILE_ATTRIBUTE_HIDDEN) : (newAttributes & ~FILE_ATTRIBUTE_HIDDEN);
	}

	if ((attributesToSet & RZFileAttributeSystem) != 0)
	{
		newAttributes = value ? (newAttributes | FILE_ATTRIBUTE_SYSTEM) : (newAttributes & ~FILE_ATTRIBUTE_SYSTEM);
	}

	if ((attributesToSet & RZFileAttributeReadOnly) != 0)
	{
		newAttributes = value ? (newAttributes | FILE_ATTRIBUTE_READONLY) : (newAttributes & ~FILE_ATTRIBUTE_READONLY);
	}

	if (newAttributes == attributes)
	{
		return true;
	}

	if (gW32FileHelpers.SetFileAttributesW != NULL)
	{
		result = gW32FileHelpers.SetFileAttributesW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0]), newAttributes) != 0;
	}
	else
	{
		result = SetFileAttributes(str.c_str(), newAttributes) != 0;
	}

	return result;
}

bool cRZFile::Remove(cIGZString const& path)
{
	if (gW32FileHelpers.DeleteFileW != NULL)
	{
		bool result = gW32FileHelpers.DeleteFileW(reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0])) != FALSE;
		return result;
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(path, str, kGZCodePageSystem);

		bool result = DeleteFile(str.Data()) != 0;
		return result;
	}
}

bool cRZFile::Rename(cIGZString const& srcPath, cIGZString const& destPath)
{
	if (gW32FileHelpers.MoveFileExW != NULL)
	{
		bool result = gW32FileHelpers.MoveFileExW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(srcPath)[0]),
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(destPath)[0]),
			MOVEFILE_COPY_ALLOWED) != 0;

		return result;
	}
	else
	{
		cRZString srcStr;
		cRZString destStr;

		ConvertStringEncoding(srcPath, srcStr, kGZCodePageSystem);
		ConvertStringEncoding(destPath, destStr, kGZCodePageSystem);

		bool result = MoveFile(srcStr.Data(), destStr.Data()) != 0;
		return result;
	}
}

bool cRZFile::Copy(cIGZString const& srcPath, cIGZString const& destPath, bool overwrite)
{
	if (gW32FileHelpers.CopyFileW != NULL)
	{
		bool result = gW32FileHelpers.CopyFileW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(srcPath)[0]),
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(destPath)[0]),
			!overwrite) != 0;

		return result;
	}
	else
	{
		cRZString srcStr;
		cRZString destStr;

		ConvertStringEncoding(srcPath, srcStr, kGZCodePageSystem);
		ConvertStringEncoding(destPath, destStr, kGZCodePageSystem);

		bool result = CopyFile(srcStr.Data(), destStr.Data(), !overwrite) != 0;
		return result;
	}
}

int32_t cRZFile::GetLength(cIGZString const& path)
{
	int32_t length = -1;
	if (gW32FileHelpers.FindFirstFileW != NULL)
	{
		WIN32_FIND_DATAW findData;
		HANDLE hFindHandle = gW32FileHelpers.FindFirstFileW(
			reinterpret_cast<wchar_t const*>(&RZStringToUTF16(path)[0]),
			&findData);

		if (hFindHandle != INVALID_HANDLE_VALUE)
		{
			FindClose(hFindHandle);
			length = (int32_t)findData.nFileSizeLow;
		}
	}
	else
	{
		cRZString str;
		ConvertStringEncoding(path, str, kGZCodePageSystem);

		WIN32_FIND_DATAA findData;
		HANDLE hFindHandle = FindFirstFile(str.Data(), &findData);

		if (hFindHandle != NULL)
		{
			FindClose(hFindHandle);
			length = (int32_t)findData.nFileSizeLow;
		}
	}

	return length;
}

bool cRZFile::GetPathExtension(cIGZString const& path, cIGZString& extension)
{
	char drive[_MAX_DRIVE] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	_splitpath(path.ToChar(), drive, dir, fname, ext);

	size_t extLen = strlen(ext);
	extension.FromChar(ext, extLen);

	return extLen != 0;
}

bool cRZFile::GetPathDirectory(cIGZString const& path, cIGZString& directory)
{
	char drive[_MAX_DRIVE] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	cRZString tmp;
	_splitpath(path.ToChar(), drive, dir, fname, ext);

	tmp = cRZString(drive);
	tmp += dir;

	directory.FromChar(tmp.c_str(), tmp.size());
	return tmp.size() != 0;
}

bool cRZFile::GetPathName(cIGZString const& path, cIGZString& name, bool includeSeparator)
{
	char drive[_MAX_DRIVE] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	cRZString tmp;
	_splitpath(path.ToChar(), drive, dir, fname, ext);

	tmp = cRZString(fname);
	if (includeSeparator)
	{
		tmp += ext;
	}

	name.FromChar(tmp.c_str(), tmp.size());
	return tmp.size() != 0;
}

bool cRZFile::GetVolumeName(cIGZString const& path, cIGZString& volume)
{
	cRZString root;
	if (!GetPathVolumeRoot(path, root))
	{
		return false;
	}

	char volumeName[MAX_PATH];
	char fileSystemName[MAX_PATH];
	DWORD serialNumber, maxComponentLength, flags;

	if (GetVolumeInformation(root.ToChar(), volumeName, MAX_PATH, &serialNumber, &maxComponentLength, &flags, fileSystemName, MAX_PATH) == 0)
	{
		return false;
	}

	cRZString tmp(volumeName);
	ConvertStringEncoding(volume, kGZCodePageSystem, tmp);
	return true;
}

bool cRZFile::GetPathVolumeRoot(cIGZString const& path, cIGZString& root)
{
	char drive[_MAX_DRIVE] = "";
	char dir[_MAX_DIR] = "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";

	_splitpath(path.ToChar(), drive, dir, fname, ext);

	size_t driveLen = strlen(drive);
	if (driveLen != 0)
	{
		drive[_MAX_DRIVE - 1] = '\\';
		drive[_MAX_DRIVE] = '\0';

		root.FromChar(drive, _MAX_DRIVE);
	}

	return driveLen != 0;
}

bool cRZFile::IsFullPath(cRZString const& path)
{
	return IsFullPath(path.c_str());
}

bool cRZFile::IsFullPath(char const* path)
{
	if ((isalpha(path[0]) && path[1] == ':' && path[2] == '\\')
		|| (path[0] == '\\' && path[1] == '\\'))
	{
		if (strstr(path, "\\.") == NULL && strstr(path, "/.") == NULL)
		{
			return true;
		}

		return true;
	}

	return false;
}