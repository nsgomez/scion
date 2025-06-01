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

	virtual void WriteBool(bool value) = 0;
	virtual void WriteSint8(int8_t value) = 0;
	virtual void WriteUint8(uint8_t value) = 0;
	virtual void WriteSint16(int16_t value) = 0;
	virtual void WriteUint16(uint16_t value) = 0;
	virtual void WriteSint32(int32_t value) = 0;
	virtual void WriteUint32(uint32_t value) = 0;
	virtual void WriteFloat32(float value) = 0;
	virtual void WriteFloat64(double value) = 0;

	virtual void WriteBoolArray(bool const* array, size_t length) = 0;
	virtual void WriteSint8Array(int8_t const* array, size_t length) = 0;
	virtual void WriteUint8Array(uint8_t const* array, size_t length) = 0;
	virtual void WriteSint16Array(int16_t const* array, size_t length) = 0;
	virtual void WriteUint16Array(uint16_t const* array, size_t length) = 0;
	virtual void WriteSint32Array(int32_t const* array, size_t length) = 0;
	virtual void WriteUint32Array(uint32_t const* array, size_t length) = 0;
	virtual void WriteFloat32Array(float const* array, size_t length) = 0;
	virtual void WriteFloat64Array(double const* array, size_t length) = 0;

	virtual void WriteString(cIGZString const* str) = 0;
	virtual void WriteString(char const* str) = 0;
	virtual void WriteObject(cIGZPersistSerializable2 const* obj) = 0;
	virtual void WriteObjectRef(cIGZPersistSerializableReferent const* obj) = 0;
	virtual void WriteObjectRef(cIGZUnknown const* obj) = 0;
	virtual void WriteTypeInfo(cGZPersistTypeInfo const& info) = 0;

	virtual cIGZOStream* OutputStream() = 0;
};