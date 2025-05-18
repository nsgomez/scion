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

#include "cGZDBReadRecordPackedFile.h"
#include "cGZDBSegmentPackedFile.h"

cGZDBReadRecordPackedFile::cGZDBReadRecordPackedFile(uint32_t recordOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian) :
	cGZPersistDBSerialRecord(key, segment, useLittleEndian),
	recordOffset(recordOffset),
	position(0),
	size(size)
{
	accessFlags = GZFileAccessRead;
}

bool cGZDBReadRecordPackedFile::GetFieldVoid(void* data, uint32_t size)
{
	if (!accessFlags || this->size < position + size)
	{
		return false;
	}

	cGZDBSegmentPackedFile* file = static_cast<cGZDBSegmentPackedFile*>(&*this->segment);
	if (file->ReadFileSpan(data, recordOffset + position, size))
	{
		position += size;
		return true;
	}

	return false;
}

bool cGZDBReadRecordPackedFile::SetFieldVoid(void const* data, uint32_t size)
{
	return false;
}

uint32_t cGZDBReadRecordPackedFile::GetSize(void)
{
	return size;
}

bool cGZDBReadRecordPackedFile::SetSize(size_t size)
{
	return false;
}

uint32_t cGZDBReadRecordPackedFile::GetPosition(void)
{
	return position;
}

bool cGZDBReadRecordPackedFile::SeekAbsolute(uint32_t position)
{
	if (!accessFlags || size <= position)
	{
		return false;
	}

	this->position = position;
	return true;
}

bool cGZDBReadRecordPackedFile::SeekRelative(int32_t offset)
{
	if (!accessFlags || size <= offset + position)
	{
		return false;
	}

	this->position += offset;
	return true;
}

void cGZDBReadRecordPackedFile::DoPostClose(void)
{
	accessFlags = 0;
	segment = NULL;
	position = 0;
}