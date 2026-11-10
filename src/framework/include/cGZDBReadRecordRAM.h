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

#pragma once
#include "cGZDBRecord.h"
#include "SharedMemoryPointer.h"

class cGZDBSegmentPackedFile;

class cGZDBReadRecordRAM : public cGZDBRecord
{
public:
	cGZDBReadRecordRAM(SharedMemoryPointer* data, int32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian);
	cGZDBReadRecordRAM(void* data, int32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian);
	virtual ~cGZDBReadRecordRAM(void);

	friend class cGZDBSegmentPackedFile;

public:
	virtual bool GetFieldVoid(void* data, uint32_t size);
	virtual void DoPostClose(void);

	virtual bool SetFieldVoid(void const* data, uint32_t size);

	virtual uint32_t GetSize(void);
	virtual bool SetSize(size_t size);

	virtual uint32_t GetPosition(void);
	virtual bool SeekAbsolute(uint32_t position);
	virtual bool SeekRelative(int32_t offset);

protected:
	SharedMemoryPointer* mData;
	int32_t mPosition;
	int32_t mSize;
};