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

	virtual bool ReadBool(void) = 0;
	virtual int8_t ReadSint8(void) = 0;
	virtual uint8_t ReadUint8(void) = 0;
	virtual int16_t ReadSint16(void) = 0;
	virtual uint16_t ReadUint16(void) = 0;
	virtual int32_t ReadSint32(void) = 0;
	virtual uint32_t ReadUint32(void) = 0;
	virtual float ReadFloat32(void) = 0;
	virtual double ReadFloat64(void) = 0;

	virtual void ReadBoolArray(bool* array, size_t length) = 0;
	virtual void ReadSint8Array(int8_t* array, size_t length) = 0;
	virtual void ReadUint8Array(uint8_t* array, size_t length) = 0;
	virtual void ReadSint16Array(int16_t* array, size_t length) = 0;
	virtual void ReadUint16Array(uint16_t* array, size_t length) = 0;
	virtual void ReadSint32Array(int32_t* array, size_t length) = 0;
	virtual void ReadUint32Array(uint32_t* array, size_t length) = 0;
	virtual void ReadFloat32Array(float* array, size_t length) = 0;
	virtual void ReadFloat64Array(double* array, size_t length) = 0;

	virtual void ReadString(cIGZString* str) = 0;
	virtual void SkipBytes(size_t length) = 0;
	virtual void ReadObject(GZCLSID clsid, void** out) = 0;
	virtual void ReadObjectRef(GZCLSID clsid, void** out) = 0;
	virtual cGZPersistTypeInfo ReadTypeInfo(void) = 0;

	virtual cIGZIStream* InputStream(void) = 0;
};