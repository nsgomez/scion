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

#include "cGZDBReadRecordRAM.h"
#include "cGZDBSegmentPackedFile.h"

cGZDBReadRecordRAM::cGZDBReadRecordRAM(SharedMemoryPointer* data, int32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBReadRecordTypeRAM, key, segment, useLittleEndian),
	mPosition(0),
	mData(data),
	mSize(size)
{
	mData->AddRef();
	mAccessFlags = GZFileAccessRead;
}

cGZDBReadRecordRAM::cGZDBReadRecordRAM(void* data, int32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBReadRecordTypeRAM, key, segment, useLittleEndian),
	mPosition(0),
	mData(new SharedMemoryPointer(data)),
	mSize(size)
{
	mData->AddRef();
	mAccessFlags = GZFileAccessRead;
}

cGZDBReadRecordRAM::~cGZDBReadRecordRAM(void)
{
	if (mData != NULL)
	{
		mData->Release();
		mData = NULL;
	}
}

bool cGZDBReadRecordRAM::GetFieldVoid(void* data, uint32_t size)
{
	if (mAccessFlags)
	{
		int32_t newPos = mPosition + size;
		if (newPos <= mSize)
		{
			memcpy(data, mData->data + mPosition, size);
			mPosition = newPos;

			return true;
		}
	}

	return false;
}

void cGZDBReadRecordRAM::DoPostClose(void)
{
	mAccessFlags = 0;

	if (mSegment != NULL)
	{
		mSegment->Release();
		mSegment = NULL;
	}

	if (mData != NULL)
	{
		mData->Release();
		mData = NULL;
	}

	mPosition = 0;
}

bool cGZDBReadRecordRAM::SetFieldVoid(void const* data, uint32_t size)
{
	if (!mAccessFlags || mSize < (mPosition + (int32_t)size))
	{
		return false;
	}

	memcpy(mData->data + mPosition, data, size);
	mPosition += (int32_t)size;

	return true;
}

uint32_t cGZDBReadRecordRAM::GetSize(void)
{
	return (uint32_t)mSize;
}

bool cGZDBReadRecordRAM::SetSize(size_t size)
{
	return false;
}

uint32_t cGZDBReadRecordRAM::GetPosition(void)
{
	return (uint32_t)mPosition;
}

bool cGZDBReadRecordRAM::SeekAbsolute(uint32_t position)
{
	if (!mAccessFlags || mSize <= (int32_t)position)
	{
		return false;
	}

	mPosition = (int32_t)position;
	return true;
}

bool cGZDBReadRecordRAM::SeekRelative(int32_t offset)
{
	if (!mAccessFlags)
	{
		return false;
	}

	int32_t newPosition = mPosition + offset;
	if (newPosition < 0 || mSize <= newPosition)
	{
		return false;
	}

	mPosition = newPosition;
	return true;
}