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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistWriter = 0x1B0AAB0E;

struct cGZPersistTypeInfo;
class cIGZOStream;
class cIGZPersistResource2;
class cIGZPersistSerializable2;
class cIGZPersistSerializableReferent;
class cIGZString;

class cIGZPersistWriter : public cIGZUnknown
{
public:
	virtual void WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream) = 0;

	virtual void WriteBool(char const* name, bool value) = 0;
	virtual void WriteSint8(char const* name, int8_t value) = 0;
	virtual void WriteUint8(char const* name, uint8_t value) = 0;
	virtual void WriteSint16(char const* name, int16_t value) = 0;
	virtual void WriteUint16(char const* name, uint16_t value) = 0;
	virtual void WriteSint32(char const* name, int32_t value) = 0;
	virtual void WriteUint32(char const* name, uint32_t value) = 0;
	virtual void WriteFloat32(char const* name, float value) = 0;
	virtual void WriteFloat64(char const* name, double value) = 0;

	virtual void WriteBoolArray(char const* name, bool const* array, size_t length) = 0;
	virtual void WriteSint8Array(char const* name, int8_t const* array, size_t length) = 0;
	virtual void WriteUint8Array(char const* name, uint8_t const* array, size_t length) = 0;
	virtual void WriteSint16Array(char const* name, int16_t const* array, size_t length) = 0;
	virtual void WriteUint16Array(char const* name, uint16_t const* array, size_t length) = 0;
	virtual void WriteSint32Array(char const* name, int32_t const* array, size_t length) = 0;
	virtual void WriteUint32Array(char const* name, uint32_t const* array, size_t length) = 0;
	virtual void WriteFloat32Array(char const* name, float const* array, size_t length) = 0;
	virtual void WriteFloat64Array(char const* name, double const* array, size_t length) = 0;

	virtual void WriteString(char const* name, cIGZString const* str) = 0;
	virtual void WriteString(char const* name, char const* str) = 0;
	virtual void WriteObject(char const* name, cIGZPersistSerializable2 const* obj) = 0;
	virtual void WriteObjectRef(char const* name, cIGZPersistSerializableReferent const* obj) = 0;
	virtual void WriteObjectRef(char const* name, cIGZUnknown const* obj) = 0;
	virtual void WriteTypeInfo(cGZPersistTypeInfo const& info) = 0;

	virtual cIGZOStream* OutputStream() = 0;
};