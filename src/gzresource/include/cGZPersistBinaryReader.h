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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <deque>
#include <vector>
#include "cGZPersistTypeInfo.h"
#include "cIGZPersistReader.h"

static const GZCLSID GZCLSID_cGZPersistBinaryReader = 0x5B101E6B;

class cGZPersistBinaryReader : public cIGZPersistReader
{
public:
	cGZPersistBinaryReader(void);
	virtual ~cGZPersistBinaryReader(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual void ReadRoot(cIGZPersistResource2* resource, cIGZIStream* stream);

	virtual bool ReadBool(void);
	virtual int8_t ReadSint8(void);
	virtual uint8_t ReadUint8(void);
	virtual int16_t ReadSint16(void);
	virtual uint16_t ReadUint16(void);
	virtual int32_t ReadSint32(void);
	virtual uint32_t ReadUint32(void);
	virtual float ReadFloat32(void);
	virtual double ReadFloat64(void);

	virtual void ReadBoolArray(bool* array, size_t length);
	virtual void ReadSint8Array(int8_t* array, size_t length);
	virtual void ReadUint8Array(uint8_t* array, size_t length);
	virtual void ReadSint16Array(int16_t* array, size_t length);
	virtual void ReadUint16Array(uint16_t* array, size_t length);
	virtual void ReadSint32Array(int32_t* array, size_t length);
	virtual void ReadUint32Array(uint32_t* array, size_t length);
	virtual void ReadFloat32Array(float* array, size_t length);
	virtual void ReadFloat64Array(double* array, size_t length);

	virtual void ReadString(cIGZString* str);
	virtual void SkipBytes(size_t length);
	virtual void ReadObject(GZCLSID clsid, void** out);
	virtual void ReadObjectRef(GZCLSID clsid, void** out);
	virtual cGZPersistTypeInfo ReadTypeInfo(void);

	virtual cIGZIStream* InputStream(void);

protected:
	struct cResourceInfo
	{
	public:
		std::vector<cIGZUnknown*> unk1;
		std::vector<cIGZUnknown*> unk2;
		cIGZIStream* stream;
		cIGZUnknown* unk3;
	};

protected:
	int32_t refCount;
	std::deque<cResourceInfo> resources;
};