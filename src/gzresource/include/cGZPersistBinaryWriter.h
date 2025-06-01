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

#pragma once
#include <deque>
#include "cIGZPersistWriter.h"

static const GZCLSID GZCLSID_cGZPersistBinaryWriter = 0xBB100367;

class cGZPersistBinaryWriter : public cIGZPersistWriter
{
public:
	cGZPersistBinaryWriter(void);
	virtual ~cGZPersistBinaryWriter(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual void WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream);

	virtual void WriteBool(bool value);
	virtual void WriteSint8(int8_t value);
	virtual void WriteUint8(uint8_t value);
	virtual void WriteSint16(int16_t value);
	virtual void WriteUint16(uint16_t value);
	virtual void WriteSint32(int32_t value);
	virtual void WriteUint32(uint32_t value);
	virtual void WriteFloat32(float value);
	virtual void WriteFloat64(double value);

	virtual void WriteBoolArray(bool const* array, size_t length);
	virtual void WriteSint8Array(int8_t const* array, size_t length);
	virtual void WriteUint8Array(uint8_t const* array, size_t length);
	virtual void WriteSint16Array(int16_t const* array, size_t length);
	virtual void WriteUint16Array(uint16_t const* array, size_t length);
	virtual void WriteSint32Array(int32_t const* array, size_t length);
	virtual void WriteUint32Array(uint32_t const* array, size_t length);
	virtual void WriteFloat32Array(float const* array, size_t length);
	virtual void WriteFloat64Array(double const* array, size_t length);

	virtual void WriteString(cIGZString const* str);
	virtual void WriteString(char const* str);
	virtual void WriteObject(cIGZPersistSerializable2 const* obj);
	virtual void WriteObjectRef(cIGZPersistSerializableReferent const* obj);
	virtual void WriteObjectRef(cIGZUnknown const* obj);
	virtual void WriteTypeInfo(cGZPersistTypeInfo const& info);

	virtual cIGZOStream* OutputStream();

protected:
	int32_t refCount;
};