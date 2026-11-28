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
#include "cIGZFile.h"

static const GZIID GZIID_cIGZPersistDBSDFRecord = 0x856C8D04; 

struct cGZPersistResourceKey;

class cIGZPersistDBSDFRecord : public cIGZUnknown
{
public:
	virtual uint32_t GetSize() = 0;

	virtual bool GetFieldVoid(void* data, size_t length) = 0;
	virtual bool SetFieldVoid(void const* data, size_t length) = 0;

	virtual uint32_t Read(void* data, uint32_t byteCount) = 0;
	virtual uint32_t Write(void const* data, uint32_t byteCount) = 0;

	virtual bool Tell(uint32_t& position) = 0;
	virtual bool Seek(uint32_t offset, int32_t method) = 0;

	virtual void Flush() = 0;

	virtual bool GetPath(cIGZString** outPath) = 0;
	virtual bool GetName(cIGZString** outName) = 0;
	virtual bool GetExtension(cIGZString** outExtension) = 0;

	virtual bool Init(cGZPersistResourceKey const& key, cIGZString const& path, GZFileAccessFlags flags) = 0;

	virtual bool CloseInternal() = 0;
	virtual bool AbortInternal() = 0;
};