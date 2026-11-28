/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cGZPersistDBSerialRecord.h"
#include "cIGZPersistDBSDFRecord.h"
#include "cRZFile.h"

class cGZDBSDFRecord : public cIGZPersistDBSDFRecord, public cGZPersistDBSerialRecord
{
public:
	cGZDBSDFRecord(cIGZPersistDBSegment* segment);
	virtual ~cGZDBSDFRecord();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual uint32_t GetSize();
	virtual bool SetSize(uint32_t size);

	virtual bool GetFieldVoid(void* data, size_t length);
	virtual bool SetFieldVoid(void const* data, size_t length);

	virtual uint32_t GetPosition();
	virtual bool SeekAbsolute(uint32_t position);
	virtual bool SeekRelative(int32_t offset);

	virtual uint32_t Read(void* data, uint32_t byteCount);
	virtual uint32_t Write(void const* data, uint32_t byteCount);

	virtual bool Tell(uint32_t& position);
	virtual bool Seek(uint32_t offset, int32_t method);

	virtual void Flush();

	virtual bool GetPath(cIGZString** outPath);
	virtual bool GetName(cIGZString** outName);
	virtual bool GetExtension(cIGZString** outExtension);

	virtual bool Init(cGZPersistResourceKey const& key, cIGZString const& path, GZFileAccessFlags flags);

	virtual bool CloseInternal();
	virtual bool AbortInternal();

	friend class cGZDBSegmentDirectoryFiles;

protected:
	bool mOpen;
	cRZFile mFile;
	cRZFile mTempFile;
	uint32_t mMode;
};