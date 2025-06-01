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

#include "cGZDBSegmentPackedFile.h"
#include "cGZDBWriteRecordPackedFile.h"

static const size_t bufferSize = 16384;

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint8_t* data, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBWriteRecordPackedFile, key, segment, useLittleEndian),
	buffer(),
	bufferPos(0),
	tmpFile(NULL),
	triedCreatingTmpFile(false)
{
}

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint32_t parentOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBWriteRecordPackedFile, key, segment, useLittleEndian),
	buffer(),
	bufferPos(0),
	tmpFile(NULL),
	triedCreatingTmpFile(false)
{
	if (size > 2000000 && !triedCreatingTmpFile)
	{
		bool tmpFileReady = false;
		cRZString tmpFilePath;
		triedCreatingTmpFile = true;

		tmpFilePath.Sprintf("Record_%08x_", key.type);
		if (cRZFile::MakeTempPathName(tmpFilePath, NULL))
		{
			tmpFile = new cRZFile(tmpFilePath);
			if (tmpFile->Open(GZFileAccessReadWrite, GZFileCreateNewOpenExisting, GZFileShareRead))
			{
				uint8_t* tmpBuf = new uint8_t[bufferSize];
				cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(segment);

				tmpFile->SeekToPosition(0);
				tmpFileReady = true;

				uint32_t offset = parentOffset;
				while (true)
				{
					if (offset >= size || !tmpFileReady)
					{
						break;
					}

					uint32_t bytesToRead = size - offset;
					if (offset + bufferSize <= size)
					{
						bytesToRead = bufferSize;
					}

					tmpFileReady = packedFile->ReadFileSpan(tmpBuf, offset, bytesToRead);
					if (tmpFileReady)
					{
						tmpFileReady = tmpFile->Write(tmpBuf, bytesToRead);
						offset += bytesToRead;
					}
				}

				if (tmpFileReady)
				{
					tmpFile->SeekToPosition(bufferPos);
				}

				delete[] tmpBuf;
			}

			if (!tmpFileReady)
			{
				if (tmpFile->IsOpen())
				{
					tmpFile->Close();
					cRZFile::Remove(tmpFile->GetPath());
				}

				delete tmpFile;
				tmpFile = NULL;
			}
		}
	}

	if (tmpFile == NULL)
	{
		cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(segment);
		buffer.resize(size);
		packedFile->ReadFileSpan(&buffer[0], parentOffset, size);
	}
}

cGZDBWriteRecordPackedFile::~cGZDBWriteRecordPackedFile(void)
{
	if (tmpFile != NULL)
	{
		if (tmpFile->IsOpen())
		{
			tmpFile->Close();
			cRZFile::Remove(tmpFile->GetPath());
		}

		delete tmpFile;
		tmpFile = NULL;
	}
}

bool cGZDBWriteRecordPackedFile::GetFieldVoid(void* data, uint32_t size)
{
	if (!accessFlags)
	{
		return false;
	}
	else if (tmpFile != NULL)
	{
		return tmpFile->Read(data, size);
	}
	else if (bufferPos + size > buffer.size())
	{
		return false;
	}

	void* src = &buffer[bufferPos];
	if (size < 32)
	{
		memcpy(data, src, size);
	}
	else if (size > 0)
	{
		memmove(data, src, size);
	}

	bufferPos += size;
	return true;
}

bool cGZDBWriteRecordPackedFile::SetFieldVoid(void const* data, uint32_t size)
{
	if (!accessFlags)
	{
		return false;
	}
	else if (tmpFile != NULL)
	{
		return tmpFile->Write(data, size);
	}

	if (bufferPos + size > 2000000 && !triedCreatingTmpFile)
	{
		bool tmpFileReady = false;
		cRZString tmpFilePath;
		triedCreatingTmpFile = true;

		tmpFilePath.Sprintf("Record_%08x_", key.type);
		if (cRZFile::MakeTempPathName(tmpFilePath, NULL))
		{
			tmpFile = new cRZFile(tmpFilePath);
			if (tmpFile->Open(GZFileAccessReadWrite, GZFileCreateNewOpenExisting, GZFileShareRead))
			{
				tmpFile->SeekToPosition(0);
				tmpFile->SetLength(0);
				if (tmpFile->Write(&buffer[0], buffer.size()))
				{
					tmpFile->Write(data, size);
					buffer.clear();

					tmpFileReady = true;
					bufferPos = 0;

					return true;
				}
			}

			if (!tmpFileReady)
			{
				if (tmpFile->IsOpen())
				{
					tmpFile->Close();
					cRZFile::Remove(tmpFile->GetPath());
				}

				delete tmpFile;
				tmpFile = NULL;
			}
		}
	}

	if (tmpFile == NULL)
	{
		if (bufferPos == buffer.size() && size < 64)
		{
			uint8_t const* dataBytes = reinterpret_cast<uint8_t const*>(data);
			for (int i = 0; i < size; i++)
			{
				buffer.push_back(*(dataBytes++));
			}
		}
		else
		{
			if (buffer.size() < bufferPos + size)
			{
				buffer.resize(bufferPos + size);
			}

			if (size < 32)
			{
				memcpy(&buffer[bufferPos], data, size);
			}
			else if (size > 0)
			{
				memmove(&buffer[bufferPos], data, size);
			}
		}

		bufferPos += size;
		return true;
	}

	return false;
}

uint32_t cGZDBWriteRecordPackedFile::GetSize(void)
{
	if (!accessFlags)
	{
		return 0;
	}
	else if (tmpFile == NULL)
	{
		return buffer.size();
	}
	else
	{
		return tmpFile->Length();
	}
}

bool cGZDBWriteRecordPackedFile::SetSize(size_t size)
{
	if (!accessFlags)
	{
		return false;
	}
	else if (tmpFile == NULL)
	{
		buffer.resize(size, '\0');
		return true;
	}
	else
	{
		return tmpFile->SetLength(size);
	}
}

uint32_t cGZDBWriteRecordPackedFile::GetPosition(void)
{
	if (!accessFlags)
	{
		return 0;
	}
	else if (tmpFile == NULL)
	{
		return bufferPos;
	}
	else
	{
		return tmpFile->Position();
	}
}

bool cGZDBWriteRecordPackedFile::SeekAbsolute(uint32_t position)
{
	if (!accessFlags)
	{
		return false;
	}
	else if (tmpFile == NULL)
	{
		bufferPos = position;
		return true;
	}
	else
	{
		return tmpFile->SeekToPosition(position) == position;
	}
}

bool cGZDBWriteRecordPackedFile::SeekRelative(int32_t offset)
{
	uint32_t size = GetSize();
	return size + offset >= 0
		&& SeekAbsolute(size + offset);
}

void cGZDBWriteRecordPackedFile::DoPostClose(void)
{
	accessFlags = 0;
	segment = NULL;

	if (tmpFile != NULL)
	{
		if (tmpFile->IsOpen())
		{
			tmpFile->Close();
			cRZFile::Remove(tmpFile->GetPath());
		}

		delete tmpFile;
		tmpFile = NULL;
	}

	buffer.clear();
	bufferPos = 0;
	triedCreatingTmpFile = false;
}

bool cGZDBWriteRecordPackedFile::WriteRecordToParent(uint32_t offset, uint32_t size)
{
	if (!accessFlags)
	{
		return false;
	}

	if (tmpFile != NULL)
	{
		uint32_t tmpOffset = offset;
		uint32_t position = tmpFile->Position();
		uint8_t* tmpData = new uint8_t[bufferSize];

		tmpFile->SeekToPosition(0);

		bool haveData = true;
		uint32_t totalBytesRead = 0;

		while (true)
		{
			uint32_t bytesToRead = size - totalBytesRead;
			if (totalBytesRead + bufferSize <= size)
			{
				bytesToRead = bufferSize;
			}

			bool ioResult = true;
			do
			{
				if (size <= totalBytesRead || !ioResult)
				{
					delete[] tmpData;
					tmpFile->SeekToPosition(position);
				}

				ioResult = tmpFile->Read(tmpData, bytesToRead);
			}
			while (ioResult);

			cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(&*segment);
			packedFile->WriteFileSpan(tmpData, tmpOffset, bytesToRead);

			totalBytesRead += bytesToRead;
			tmpOffset += bytesToRead;
		}
	}

	cGZDBSegmentPackedFile* packedFile = static_cast<cGZDBSegmentPackedFile*>(&*segment);
	uint32_t bytesToWrite = buffer.size();

	if (size <= bytesToWrite)
	{
		bytesToWrite = size;
	}

	return packedFile->WriteFileSpan(&buffer[0], offset, bytesToWrite);
}