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

static const GZIID GZIID_cIGZPersistReader = 0xFB0C4344;

class cIGZPersistResource2;
class cIGZIStream;
class cIGZString;

class cIGZPersistReader : public cIGZUnknown
{
public:
	virtual void ReadRoot(cIGZPersistResource2* resource, cIGZIStream* stream) = 0;

	virtual bool ReadBool(char const* name) = 0;
	virtual int8_t ReadSint8(char const* name) = 0;
	virtual uint8_t ReadUint8(char const* name) = 0;
	virtual int16_t ReadSint16(char const* name) = 0;
	virtual uint16_t ReadUint16(char const* name) = 0;
	virtual int32_t ReadSint32(char const* name) = 0;
	virtual uint32_t ReadUint32(char const* name) = 0;
	virtual float ReadFloat32(char const* name) = 0;
	virtual double ReadFloat64(char const* name) = 0;

	virtual void ReadBoolArray(char const* name, bool* array, size_t length) = 0;
	virtual void ReadSint8Array(char const* name, int8_t* array, size_t length) = 0;
	virtual void ReadUint8Array(char const* name, uint8_t* array, size_t length) = 0;
	virtual void ReadSint16Array(char const* name, int16_t* array, size_t length) = 0;
	virtual void ReadUint16Array(char const* name, uint16_t* array, size_t length) = 0;
	virtual void ReadSint32Array(char const* name, int32_t* array, size_t length) = 0;
	virtual void ReadUint32Array(char const* name, uint32_t* array, size_t length) = 0;
	virtual void ReadFloat32Array(char const* name, float* array, size_t length) = 0;
	virtual void ReadFloat64Array(char const* name, double* array, size_t length) = 0;

	virtual void ReadString(char const* name, cIGZString* str) = 0;
	virtual void SkipBytes(char const* name, size_t length) = 0;
	virtual void ReadObject(char const* name, GZIID iid, void** out) = 0;
	virtual void ReadObjectRef(char const* name, GZIID iid, void** out) = 0;
	virtual cGZPersistTypeInfo ReadTypeInfo(void) = 0;

	virtual cIGZIStream* InputStream(void) = 0;
};