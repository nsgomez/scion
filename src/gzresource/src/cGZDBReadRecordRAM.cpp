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

#include "cGZDBReadRecordRAM.h"
#include "cGZDBSegmentPackedFile.h"

// TODO: create + use SharedMemoryPointer type

cGZDBReadRecordRAM::cGZDBReadRecordRAM(uint8_t* data, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZDBRecord(DBReadRecordTypeRAM, key, segment, useLittleEndian),
	data(data),
	position(0),
	size(size)
{
	accessFlags = GZFileAccessRead;
}

cGZDBReadRecordRAM::~cGZDBReadRecordRAM(void)
{
	// TODO: release SharedMemoryPointer once implemented
}

bool cGZDBReadRecordRAM::GetFieldVoid(void* data, uint32_t size)
{
	if (!accessFlags || this->size < position + size)
	{
		return false;
	}

	uint8_t const* src = this->data + position;
	if (size < 32)
	{
		memcpy(data, src, size);
	}
	else if (size > 0)
	{
		memmove(data, src, size);
	}

	position += size;
	return true;
}

bool cGZDBReadRecordRAM::SetFieldVoid(void const* data, uint32_t size)
{
	if (!accessFlags || this->size < position + size)
	{
		return false;
	}

	uint8_t* dest = this->data + position;
	memcpy(dest, data, size);

	position += size;
	return true;
}

uint32_t cGZDBReadRecordRAM::GetSize(void)
{
	return size;
}

bool cGZDBReadRecordRAM::SetSize(size_t size)
{
	return false;
}

uint32_t cGZDBReadRecordRAM::GetPosition(void)
{
	return position;
}

bool cGZDBReadRecordRAM::SeekAbsolute(uint32_t position)
{
	if (!accessFlags || size <= position)
	{
		return false;
	}

	this->position = position;
	return true;
}

bool cGZDBReadRecordRAM::SeekRelative(int32_t offset)
{
	if (!accessFlags)
	{
		return false;
	}

	int32_t newPosition = offset + position;
	if (newPosition < 0 || size <= newPosition)
	{
		return false;
	}

	position = newPosition;
	return true;
}

void cGZDBReadRecordRAM::DoPostClose(void)
{
	accessFlags = 0;
	segment = NULL;
	data = NULL;
	position = 0;
}