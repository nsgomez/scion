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

#include "cGZDBSegmentPackedFile.h"
#include "cGZDBWriteRecordPackedFile.h"

static const size_t kDefaultBufferSize = 16384;

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint8_t* data, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBWriteRecordPackedFile, key, segment, useLittleEndian),
	mBuffer(),
	mBufferPos(0),
	mTmpFile(NULL),
	mTriedCreatingTmpFile(false)
{
	mAccessFlags = GZFileAccessReadWrite;
	mBuffer.resize(size);
	memcpy(&mBuffer[0], data, size);
}

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint32_t parentOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBWriteRecordPackedFile, key, segment, useLittleEndian),
	mBuffer(),
	mBufferPos(0),
	mTmpFile(NULL),
	mTriedCreatingTmpFile(false)
{
	mAccessFlags = GZFileAccessReadWrite;
	mBuffer.reserve(128);

	if (size != 0)
	{
		if (size > 2000000 && !mTriedCreatingTmpFile)
		{
			bool tmpFileReady = false;
			cRZString tmpFilePath;
			mTriedCreatingTmpFile = true;

			tmpFilePath.Sprintf("Record_%08x_", key.type);
			if (cRZFile::MakeTempPathName(tmpFilePath, NULL))
			{
				mTmpFile = new cRZFile(tmpFilePath);
				if (mTmpFile->Open(GZFileAccessReadWrite, GZFileCreateNewOpenExisting, GZFileShareRead))
				{
					uint8_t* tmpBuf = new uint8_t[kDefaultBufferSize];
					cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(segment);

					mTmpFile->SeekToPosition(0);
					tmpFileReady = true;

					uint32_t offset = parentOffset;
					while (true)
					{
						if (offset >= size || !tmpFileReady)
						{
							break;
						}

						uint32_t bytesToRead = size - offset;
						if (offset + kDefaultBufferSize <= size)
						{
							bytesToRead = kDefaultBufferSize;
						}

						tmpFileReady = packedFile->ReadFileSpan(tmpBuf, offset, bytesToRead);
						if (tmpFileReady)
						{
							tmpFileReady = mTmpFile->Write(tmpBuf, bytesToRead);
							offset += bytesToRead;
						}
					}

					if (tmpFileReady)
					{
						mTmpFile->SeekToPosition(mBufferPos);
					}

					delete[] tmpBuf;
				}

				if (!tmpFileReady)
				{
					if (mTmpFile->IsOpen())
					{
						mTmpFile->Close();
						cRZFile::Remove(mTmpFile->GetPath());
					}

					delete mTmpFile;
					mTmpFile = NULL;
				}
			}
		}

		if (mTmpFile == NULL)
		{
			cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(segment);
			mBuffer.resize(size);
			packedFile->ReadFileSpan(&mBuffer[0], parentOffset, size);
		}
	}
}

cGZDBWriteRecordPackedFile::~cGZDBWriteRecordPackedFile(void)
{
	if (mTmpFile != NULL)
	{
		if (mTmpFile->IsOpen())
		{
			mTmpFile->Close();
			cRZFile::Remove(mTmpFile->GetPath());
		}

		delete mTmpFile;
		mTmpFile = NULL;
	}
}

bool cGZDBWriteRecordPackedFile::GetFieldVoid(void* data, uint32_t size)
{
	if (mAccessFlags != 0)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->Read(data, size);
		}

		if (mBufferPos + size <= mBuffer.size())
		{
			memcpy(data, &mBuffer[mBufferPos], size);
			mBufferPos += size;
			return true;
		}
	}

	return false;
}

bool cGZDBWriteRecordPackedFile::SetFieldVoid(void const* data, uint32_t size)
{
	bool result = false;
	if (mAccessFlags != 0)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->Write(data, size);
		}

		int32_t newEnd = (int32_t)(mBufferPos + size);
		if (newEnd >= 2000000 && !mTriedCreatingTmpFile)
		{
			bool tmpFileReady = false;

			cRZString tmpFilePath;
			mTriedCreatingTmpFile = true;

			tmpFilePath.Sprintf("Record_%08x_", mKey.type);
			if (cRZFile::MakeTempPathName(tmpFilePath, NULL))
			{
				mTmpFile = new cRZFile(tmpFilePath);
				if (mTmpFile->Open(GZFileAccessReadWrite, GZFileCreateNewOpenExisting, GZFileShareRead))
				{
					mTmpFile->SeekToPosition(0);
					mTmpFile->SetLength(0);

					if (mTmpFile->Write(&mBuffer[0], mBuffer.size()))
					{
						mTmpFile->Write(data, size);
						mBuffer.resize(0);

						tmpFileReady = true;
						mBufferPos = 0;
						result = true;
					}
				}
			}

			if (!tmpFileReady)
			{
				if (mTmpFile->IsOpen())
				{
					mTmpFile->Close();
					cRZFile::Remove(mTmpFile->GetPath());
				}

				delete mTmpFile;
				mTmpFile = NULL;
			}
		}

		if (mTmpFile == NULL)
		{
			uint32_t bufSize = mBuffer.size();
			if (mBufferPos == bufSize && size < 64 && size > 0)
			{
				for (uint32_t i = 0; i < size; i++)
				{
					mBuffer.push_back((uint8_t)(static_cast<char const*>(data)[i]));
				}
			}
			else
			{
				if (newEnd > bufSize)
				{
					mBuffer.resize(mBufferPos + size, 0);
				}

				memcpy(&mBuffer[mBufferPos], data, size);
			}

			mBufferPos += size;
			result = true;
		}
	}

	return result;
}

uint32_t cGZDBWriteRecordPackedFile::GetSize(void)
{
	if (mAccessFlags)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->Length();
		}

		return mBuffer.size();
	}

	return 0;
}

bool cGZDBWriteRecordPackedFile::SetSize(size_t size)
{
	if (mAccessFlags)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->SetLength(size);
		}

		mBuffer.resize(size);
		return true;
	}

	return false;
}

uint32_t cGZDBWriteRecordPackedFile::GetPosition(void)
{
	if (mAccessFlags)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->Position();
		}

		return mBufferPos;
	}

	return 0;
}

bool cGZDBWriteRecordPackedFile::SeekAbsolute(uint32_t position)
{
	if (mAccessFlags)
	{
		if (mTmpFile != NULL)
		{
			return mTmpFile->SeekToPosition(position) == position;
		}

		mBufferPos = position;
		return true;
	}

	return false;
}

bool cGZDBWriteRecordPackedFile::SeekRelative(int32_t offset)
{
	int64_t position = (int64_t)(GetPosition() + offset);
	if (position >= 0)
	{
		return SeekAbsolute((uint32_t)position);
	}

	return false;
}

void cGZDBWriteRecordPackedFile::DoPostClose(void)
{
	mAccessFlags = 0;
	mSegment = NULL;

	if (mTmpFile != NULL)
	{
		if (mTmpFile->IsOpen())
		{
			mTmpFile->Close();
			cRZFile::Remove(mTmpFile->GetPath());
		}

		delete mTmpFile;
		mTmpFile = NULL;
	}

	mBuffer.clear();
	mBufferPos = 0;
	mTriedCreatingTmpFile = false;
}

bool cGZDBWriteRecordPackedFile::WriteRecordToParent(uint32_t offset, uint32_t size)
{
	bool succeeded = false;
	if (mAccessFlags)
	{
		if (mTmpFile != NULL)
		{
			uint32_t position = mTmpFile->Position();
			uint32_t totalBytesWritten = 0;
			uint32_t tmpOffset = offset;
			uint8_t* tmpData = new uint8_t[kDefaultBufferSize];

			mTmpFile->SeekToPosition(0);
			succeeded = true;

			while (totalBytesWritten < size && succeeded)
			{
				uint32_t bytesToWrite;
				if (totalBytesWritten + kDefaultBufferSize <= size)
				{
					bytesToWrite = kDefaultBufferSize;
				}
				else
				{
					bytesToWrite = size - totalBytesWritten;
				}

				succeeded = mTmpFile->Read(tmpData, bytesToWrite);
				if (succeeded)
				{
					cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(&*mSegment);
					succeeded = packedFile->WriteFileSpan(tmpData, tmpOffset, bytesToWrite);

					totalBytesWritten += bytesToWrite;
					tmpOffset += bytesToWrite;
				}
			}

			delete[] tmpData;
			mTmpFile->SeekToPosition(position);
		}
		else
		{
			cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(&*mSegment);

			uint32_t bytesToWrite = size;
			if (bytesToWrite > mBuffer.size())
			{
				bytesToWrite = mBuffer.size();
			}

			succeeded = packedFile->WriteFileSpan(&mBuffer[0], offset, bytesToWrite);
		}
	}

	return succeeded;
}