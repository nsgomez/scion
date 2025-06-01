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

#include "cGZDBSerialRecordAsIGZFile.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cRZString.h"

cGZDBSerialRecordAsIGZFile::cGZDBSerialRecordAsIGZFile(cIGZPersistDBSerialRecord* record) :
	refCount(0),
	record(record)
{
	if (record != NULL)
	{
		record->AddRef();
	}
}

cGZDBSerialRecordAsIGZFile::~cGZDBSerialRecordAsIGZFile()
{
	if (IsOpen())
	{
		Close();
	}
}

bool cGZDBSerialRecordAsIGZFile::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid = GZIID_cIGZDBSerialRecordAsIGZFile || iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZDBSerialRecordAsIGZFile*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZFile)
	{
		*outPtr = static_cast<cIGZFile*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZDBSerialRecordAsIGZFile::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZDBSerialRecordAsIGZFile::Release(void)
{
	if (refCount > 1)
	{
		return --refCount;
	}

	FinalRelease();
	return 0;
}

void cGZDBSerialRecordAsIGZFile::FinalRelease(void)
{
	if (IsOpen())
	{
		Close();
	}

	delete this;
}

bool cGZDBSerialRecordAsIGZFile::SetDBSerialRecord(cIGZPersistDBSerialRecord* record)
{
	if (record != NULL)
	{
		record->AddRef();
	}

	if (this->record != NULL)
	{
		this->record->Release();
	}

	this->record = record;
	return true;
}

cIGZPersistDBSerialRecord* cGZDBSerialRecordAsIGZFile::GetDBSerialRecord(void)
{
	return record;
}

cIGZFile* cGZDBSerialRecordAsIGZFile::AsIGZFile(void)
{
	return static_cast<cIGZFile*>(this);
}

bool cGZDBSerialRecordAsIGZFile::Open(uint32_t accessFlags, uint32_t securityFlags, uint32_t shareFlags)
{
	return false;
}

bool cGZDBSerialRecordAsIGZFile::IsOpen(void) const
{
	return record != NULL;
}

bool cGZDBSerialRecordAsIGZFile::Close(void)
{
	if (record != NULL)
	{
		cIGZPersistDBRecord* persistDBRecord = record->AsIGZPersistDBRecord();
		persistDBRecord->Close();

		record->Release();
		record = NULL;

		return true;
	}

	return false;
}

int32_t cGZDBSerialRecordAsIGZFile::Position(void)
{
	if (record == NULL)
	{
		return -1;
	}

	return record->AsIGZPersistDBRecord()->GetPosition();
}

int32_t cGZDBSerialRecordAsIGZFile::Length(void)
{
	if (record == NULL)
	{
		return 0;
	}

	return record->AsIGZPersistDBRecord()->GetSize();
}

bool cGZDBSerialRecordAsIGZFile::SetLength(uint32_t length)
{
	if (record == NULL)
	{
		return false;
	}

	return record->AsIGZPersistDBRecord()->SetSize(length);
}

int32_t cGZDBSerialRecordAsIGZFile::SeekToBegin(void)
{
	if (record != NULL && record->SeekAbsolute(0))
	{
		return 0;
	}

	return -1;
}

int32_t cGZDBSerialRecordAsIGZFile::SeekToEnd(void)
{
	if (record == NULL)
	{
		return -1;
	}

	int32_t size = record->GetSize();
	if (size != -1 && record->SeekAbsolute(size))
	{
		return size;
	}

	return -1;
}

int32_t cGZDBSerialRecordAsIGZFile::SeekToRelativePosition(int32_t offset)
{
	if (record != NULL && record->SeekRelative(offset))
	{
		return record->GetPosition();
	}

	return -1;
}

int32_t cGZDBSerialRecordAsIGZFile::SeekToPosition(int32_t position)
{
	if (record != NULL && record->SeekAbsolute(position))
	{
		return position;
	}

	return -1;
}

int32_t cGZDBSerialRecordAsIGZFile::Seek(int32_t offset, int32_t mode)
{
	if (record == NULL)
	{
		return -1;
	}

	if (mode == GZSeekFromBegin)
	{
		return SeekToPosition(offset);
	}
	else if (mode == GZSeekFromCurrent)
	{
		return SeekToRelativePosition(offset);
	}
	else if (mode == GZSeekFromEnd)
	{
		size_t size = record->GetSize();
		if (size != 0)
		{
			return SeekToPosition(size + offset);
		}
	}

	return -1;
}

bool cGZDBSerialRecordAsIGZFile::Read(void* data, uint32_t byteCount)
{
	if (record == NULL)
	{
		return 0;
	}

	return record->GetFieldVoid(data, byteCount);
}

bool cGZDBSerialRecordAsIGZFile::ReadWithCount(void* data, uint32_t& byteCount)
{
	if (record == NULL)
	{
		return false;
	}

	uint32_t position = record->GetPosition();
	uint32_t size = record->GetSize();

	if (size < position + byteCount)
	{
		byteCount = size - position;
	}

	return record->GetFieldVoid(data, byteCount);
}

bool cGZDBSerialRecordAsIGZFile::Write(void const* data, uint32_t byteCount)
{
	if (record == NULL)
	{
		return 0;
	}

	return record->SetFieldVoid(data, byteCount);
}

bool cGZDBSerialRecordAsIGZFile::WriteWithCount(void const* data, uint32_t& byteCount)
{
	if (record == NULL)
	{
		return false;
	}

	bool result = record->SetFieldVoid(data, byteCount);
	if (!result)
	{
		byteCount = 0;
	}

	return true;
}

bool cGZDBSerialRecordAsIGZFile::Flush(void)
{
	return true;
}

bool cGZDBSerialRecordAsIGZFile::Remove(void)
{
	return false;
}

bool cGZDBSerialRecordAsIGZFile::Rename(cIGZString const& newName)
{
	return false;
}

bool cGZDBSerialRecordAsIGZFile::Exists(void)
{
	return record != NULL;
}

bool cGZDBSerialRecordAsIGZFile::Copy(cIGZFile const& srcFile, bool overwrite)
{
	return false;
}

bool cGZDBSerialRecordAsIGZFile::Copy(cIGZString const& srcName, bool overwrite)
{
	return false;
}

void cGZDBSerialRecordAsIGZFile::GetPath(cIGZString& path) const
{
	return;
}

cIGZString const& cGZDBSerialRecordAsIGZFile::GetPath(void) const
{
	static cRZString sEmpty;
	return sEmpty;
}

const char* cGZDBSerialRecordAsIGZFile::GetPathCStr(void) const
{
	return NULL;
}

void cGZDBSerialRecordAsIGZFile::SetPath(cIGZString const& path)
{
	return;
}

void cGZDBSerialRecordAsIGZFile::SetPath(char const* path)
{
	return;
}

bool cGZDBSerialRecordAsIGZFile::GetOpenModes(uint32_t& accessFlags, uint32_t& createMode, uint32_t& shareFlags)
{
	accessFlags = 0;
	createMode = GZFileCreateNewOpenExisting;
	shareFlags = GZFileShareRead;

	if (record == NULL)
	{
		return false;
	}

	uint32_t recordFlags = record->AsIGZPersistDBRecord()->GetAccessFlags();
	if (recordFlags & GZFileAccessRead)
	{
		accessFlags |= GZFileAccessRead;
	}

	if (recordFlags & GZFileAccessWrite)
	{
		accessFlags |= GZFileAccessWrite;
	}

	return true;
}

void cGZDBSerialRecordAsIGZFile::GetBuffering(uint32_t& readBufferSize, uint32_t& writeBufferSize)
{
	return;
}

bool cGZDBSerialRecordAsIGZFile::SetBuffering(uint32_t newReadBufferSize, uint32_t newWriteBufferSize)
{
	return true;
}