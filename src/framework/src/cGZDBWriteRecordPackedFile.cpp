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

#include "cGZDBSegmentPackedFile.h"
#include "cGZDBWriteRecordPackedFile.h"

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint8_t* data, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian)
{
}

cGZDBWriteRecordPackedFile::cGZDBWriteRecordPackedFile(uint32_t parentOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian)
{
}

cGZDBWriteRecordPackedFile::~cGZDBWriteRecordPackedFile(void)
{
}

bool cGZDBWriteRecordPackedFile::GetFieldVoid(void* data, uint32_t size)
{
	if (!accessFlags)
	{
		return false;
	}

}

bool cGZDBWriteRecordPackedFile::SetFieldVoid(void const* data, uint32_t size)
{
}

uint32_t cGZDBWriteRecordPackedFile::GetSize(void)
{
}

bool cGZDBWriteRecordPackedFile::SetSize(size_t size)
{
}

uint32_t cGZDBWriteRecordPackedFile::GetPosition(void)
{
}

bool cGZDBWriteRecordPackedFile::SeekAbsolute(uint32_t position)
{
}

bool cGZDBWriteRecordPackedFile::SeekRelative(int32_t offset)
{
}

void cGZDBWriteRecordPackedFile::DoPostClose(void)
{
}

bool cGZDBWriteRecordPackedFile::WriteRecordToParent(uint32_t offset, uint32_t size)
{
}