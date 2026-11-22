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
#include <map>
#include <vector>
#include "cIGZOStream.h"
#include "cIGZPersistResource2.h"
#include "cIGZPersistSerializableReferent.h"
#include "cIGZPersistSurveyorWriter.h"
#include "cRZAutoRefCount.h"

static const GZGUID GZCLSID_cGZPersistSurveyorWriter = 0xBB194C78;

class cGZPersistSurveyorWriter : public cIGZPersistSurveyorWriter
{
public:
	cGZPersistSurveyorWriter();
	virtual ~cGZPersistSurveyorWriter();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

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

public:
	virtual void Clear();
	virtual cIGZPersistResource2* Resource() const;

	virtual uint32_t NumObjectRefs() const;
	virtual cIGZPersistSerializableReferent* ObjectRef(int32_t index) const;
	virtual uint32_t IndexOfObjectRef(cIGZPersistSerializableReferent const* obj) const;

	virtual uint32_t NumDependentResources() const;
	virtual cIGZPersistResource2* DependentResource(int32_t index) const;
	virtual uint32_t IndexOfDependentResource(cIGZPersistResource2 const* resource) const;

	virtual void AddObjectRef(cIGZPersistSerializableReferent const* obj);
	virtual void AddDepResource(cIGZPersistResource2* resource);

protected:
	uint32_t mRefCount;

	cRZAutoRefCount<cIGZOStream> mStream;
	cRZAutoRefCount<cIGZPersistResource2> mResource;

	std::vector<cRZAutoRefCount<cIGZPersistSerializableReferent> > mReferents;
	std::map<cIGZPersistSerializableReferent*, uint32_t> mReferentIndexes;

	std::vector<cRZAutoRefCount<cIGZPersistResource2> > mDependentResources;
	std::map<cIGZPersistResource2*, uint32_t> mDependentResourceIndexes;
};