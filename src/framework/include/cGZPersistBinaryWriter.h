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
#include "cIGZPersistSurveyorWriter.h"
#include "cIGZPersistWriter.h"
#include "cRZAutoRefCount.h"

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

	virtual void WriteBool(char const* name, bool value);
	virtual void WriteSint8(char const* name, int8_t value);
	virtual void WriteUint8(char const* name, uint8_t value);
	virtual void WriteSint16(char const* name, int16_t value);
	virtual void WriteUint16(char const* name, uint16_t value);
	virtual void WriteSint32(char const* name, int32_t value);
	virtual void WriteUint32(char const* name, uint32_t value);
	virtual void WriteFloat32(char const* name, float value);
	virtual void WriteFloat64(char const* name, double value);

	virtual void WriteBoolArray(char const* name, bool const* array, size_t length);
	virtual void WriteSint8Array(char const* name, int8_t const* array, size_t length);
	virtual void WriteUint8Array(char const* name, uint8_t const* array, size_t length);
	virtual void WriteSint16Array(char const* name, int16_t const* array, size_t length);
	virtual void WriteUint16Array(char const* name, uint16_t const* array, size_t length);
	virtual void WriteSint32Array(char const* name, int32_t const* array, size_t length);
	virtual void WriteUint32Array(char const* name, uint32_t const* array, size_t length);
	virtual void WriteFloat32Array(char const* name, float const* array, size_t length);
	virtual void WriteFloat64Array(char const* name, double const* array, size_t length);

	virtual void WriteString(char const* name, cIGZString const* str);
	virtual void WriteString(char const* name, char const* str);
	virtual void WriteObject(char const* name, cIGZPersistSerializable2 const* obj);
	virtual void WriteObjectRef(char const* name, cIGZPersistSerializableReferent const* obj);
	virtual void WriteObjectRef(char const* name, cIGZUnknown const* obj);
	virtual void WriteTypeInfo(cGZPersistTypeInfo const& info);

	virtual cIGZOStream* OutputStream();

protected:
	struct cSurveyorList : public std::deque<cRZAutoRefCount<cIGZPersistSurveyorWriter> >
	{
	public:
		cSurveyorList();
	};

	uint32_t mRefCount;
	cSurveyorList mSurveyors;
};