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

#pragma once
#include "cGZPersistDBSerialRecord.h"

class cGZDBSegmentPackedFile;

class cGZDBReadRecordPackedFile : public cGZPersistDBSerialRecord
{
public:
	cGZDBReadRecordPackedFile(uint32_t recordOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian);
	virtual ~cGZDBReadRecordPackedFile(void) { }

public:
	virtual bool GetFieldVoid(void* data, uint32_t size);
	virtual bool SetFieldVoid(void const* data, uint32_t size);

	virtual uint32_t GetSize(void);
	virtual bool SetSize(size_t size);

	virtual uint32_t GetPosition(void);
	virtual bool SeekAbsolute(uint32_t position);
	virtual bool SeekRelative(int32_t offset);

	virtual void DoPostClose(void);

protected:
	uint32_t recordOffset;
	uint32_t position;
	uint32_t size;
};