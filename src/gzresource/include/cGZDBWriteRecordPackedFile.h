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
#include <vector>
#include "cGZDBRecord.h"

class cGZDBSegmentPackedFile;

class cGZDBWriteRecordPackedFile : public cGZDBRecord
{
public:
	cGZDBWriteRecordPackedFile(uint8_t* data, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian);
	cGZDBWriteRecordPackedFile(uint32_t parentOffset, uint32_t size, cGZPersistResourceKey const& key, cGZDBSegmentPackedFile* segment, bool useLittleEndian);
	virtual ~cGZDBWriteRecordPackedFile(void);

	friend class cGZDBSegmentPackedFile;

public:
	virtual bool GetFieldVoid(void* data, uint32_t size);
	virtual bool SetFieldVoid(void const* data, uint32_t size);

	virtual uint32_t GetSize(void);
	virtual bool SetSize(size_t size);

	virtual uint32_t GetPosition(void);
	virtual bool SeekAbsolute(uint32_t position);
	virtual bool SeekRelative(int32_t offset);

	virtual void DoPostClose(void);

public:
	virtual bool WriteRecordToParent(uint32_t offset, uint32_t size);

protected:
	std::vector<uint8_t> buffer;
	uint32_t bufferPos;
	cRZFile* tmpFile;
	bool triedCreatingTmpFile;
};