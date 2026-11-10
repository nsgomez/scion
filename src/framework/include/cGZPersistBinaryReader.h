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
#include <deque>
#include <vector>
#include "cGZPersistTypeInfo.h"
#include "cIGZIStream.h"
#include "cIGZPersistReader.h"
#include "cIGZPersistResource2.h"
#include "cIGZPersistSerializableReferent.h"
#include "cRZAutoRefCount.h"

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

	virtual bool ReadBool(char const* name);
	virtual int8_t ReadSint8(char const* name);
	virtual uint8_t ReadUint8(char const* name);
	virtual int16_t ReadSint16(char const* name);
	virtual uint16_t ReadUint16(char const* name);
	virtual int32_t ReadSint32(char const* name);
	virtual uint32_t ReadUint32(char const* name);
	virtual float ReadFloat32(char const* name);
	virtual double ReadFloat64(char const* name);

	virtual void ReadBoolArray(char const* name, bool* array, size_t length);
	virtual void ReadSint8Array(char const* name, int8_t* array, size_t length);
	virtual void ReadUint8Array(char const* name, uint8_t* array, size_t length);
	virtual void ReadSint16Array(char const* name, int16_t* array, size_t length);
	virtual void ReadUint16Array(char const* name, uint16_t* array, size_t length);
	virtual void ReadSint32Array(char const* name, int32_t* array, size_t length);
	virtual void ReadUint32Array(char const* name, uint32_t* array, size_t length);
	virtual void ReadFloat32Array(char const* name, float* array, size_t length);
	virtual void ReadFloat64Array(char const* name, double* array, size_t length);

	virtual void ReadString(char const* name, cIGZString* str);
	virtual void SkipBytes(char const* name, size_t length);
	virtual void ReadObject(char const* name, GZIID iid, void** out);
	virtual void ReadObjectRef(char const* name, GZIID iid, void** out);
	virtual cGZPersistTypeInfo ReadTypeInfo(void);

	virtual cIGZIStream* InputStream(void);

protected:
	struct cResourceInfo
	{
	public:
		cResourceInfo(cIGZIStream* stream);

	public:
		std::vector<cRZAutoRefCount<cIGZPersistResource2> > dependents;
		std::vector<cRZAutoRefCount<cIGZPersistSerializableReferent> > objectRefs;
		cRZAutoRefCount<cIGZIStream> stream;
	};

	struct cResourceInfoList : public std::deque<cResourceInfo>
	{
	public:
		cResourceInfoList();
	};

protected:
	uint32_t mRefCount;
	cResourceInfoList mResources;
};