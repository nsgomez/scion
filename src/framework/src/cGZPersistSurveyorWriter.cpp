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

#include "cGZPersistSurveyorWriter.h"
#include "cRZNullOStream.h"

cGZPersistSurveyorWriter::cGZPersistSurveyorWriter() :
	mRefCount(0),
	mStream(NULL),
	mResource(NULL)
{
}

cGZPersistSurveyorWriter::~cGZPersistSurveyorWriter()
{
}

bool cGZPersistSurveyorWriter::QueryInterface(GZIID iid, void** outPtr)
{
	if (outPtr == NULL)
	{
		return false;
	}
	else if (iid == GZIID_cIGZUnknown || iid == GZIID_cIGZPersistWriter || iid == GZIID_cIGZPersistSurveyorWriter)
	{
		*outPtr = this;
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistSurveyorWriter::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZPersistSurveyorWriter::Release()
{
	if (--mRefCount == 0)
	{
		delete this;
		return 0;
	}

	return mRefCount;
}

void cGZPersistSurveyorWriter::WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream)
{
	Clear();

	mResource = const_cast<cIGZPersistResource2*>(resource);
	AddObjectRef(const_cast<cIGZPersistResource2*>(resource));

	for (int i = 0; i < mReferents.size(); i++)
	{
		mReferents[i]->WriteTo(this);
	}

	mStream = stream;
}

void cGZPersistSurveyorWriter::WriteObject(char const* name, cIGZPersistSerializable2 const* obj)
{
	if (obj != NULL)
	{
		const_cast<cIGZPersistSerializable2*>(obj)->WriteTo(this);
	}
}

void cGZPersistSurveyorWriter::WriteObjectRef(char const* name, cIGZPersistSerializableReferent const* obj)
{
	cIGZPersistSerializableReferent* referent = const_cast<cIGZPersistSerializableReferent*>(obj);
	if (referent == NULL)
	{
		return;
	}

	cIGZPersistResource2* resource = referent->Resource();
	if (resource != mResource)
	{
		AddDepResource(resource);
	}
	else
	{
		AddObjectRef(referent);
	}
}

void cGZPersistSurveyorWriter::WriteObjectRef(char const* name, cIGZUnknown const* obj)
{
	cIGZPersistSerializableReferent* referent = NULL;
	if (obj != NULL)
	{
		const_cast<cIGZUnknown*>(obj)->QueryInterface(GZIID_cIGZPersistSerializableReferent, reinterpret_cast<void**>(&referent));
	}

	WriteObjectRef(name, referent);

	if (referent != NULL)
	{
		referent->Release();
	}
}

cIGZOStream* cGZPersistSurveyorWriter::OutputStream()
{
	if (mStream != NULL)
	{
		return mStream;
	}

	static cRZAutoRefCount<cIGZOStream> sDefaultStream(new cRZNullOStream());
	return sDefaultStream;
}

void cGZPersistSurveyorWriter::Clear()
{
	mResource = NULL;

	mReferents.clear();
	mReferentIndexes.clear();

	mDependentResources.clear();
	mDependentResourceIndexes.clear();
}

cIGZPersistResource2* cGZPersistSurveyorWriter::Resource() const
{
	return mResource;
}

uint32_t cGZPersistSurveyorWriter::NumObjectRefs() const
{
	return mReferents.size();
}

cIGZPersistSerializableReferent* cGZPersistSurveyorWriter::ObjectRef(int32_t index) const
{
	return mReferents[index];
}

uint32_t cGZPersistSurveyorWriter::IndexOfObjectRef(cIGZPersistSerializableReferent const* obj) const
{
	std::map<cIGZPersistSerializableReferent*, uint32_t>& index = const_cast<std::map<cIGZPersistSerializableReferent*, uint32_t>&>(mReferentIndexes);
	cIGZPersistSerializableReferent* key = const_cast<cIGZPersistSerializableReferent*>(obj);

	if (index.lower_bound(key) != index.end())
	{
		return index[key];
	}

	return -1;
}

uint32_t cGZPersistSurveyorWriter::NumDependentResources() const
{
	return mDependentResources.size();
}

cIGZPersistResource2* cGZPersistSurveyorWriter::DependentResource(int32_t index) const
{
	return mDependentResources[index];
}

uint32_t cGZPersistSurveyorWriter::IndexOfDependentResource(cIGZPersistResource2 const* resource) const
{
	std::map<cIGZPersistResource2*, uint32_t>& index = const_cast<std::map<cIGZPersistResource2*, uint32_t>&>(mDependentResourceIndexes);
	cIGZPersistResource2* key = const_cast<cIGZPersistResource2*>(resource);

	if (index.lower_bound(key) != index.end())
	{
		return index[key];
	}

	return -1;
}

void cGZPersistSurveyorWriter::AddObjectRef(cIGZPersistSerializableReferent const* obj)
{
	cIGZPersistSerializableReferent* ref = const_cast<cIGZPersistSerializableReferent*>(obj);
	if (mReferentIndexes.lower_bound(ref) == mReferentIndexes.end())
	{
		mReferents.push_back(cRZAutoRefCount<cIGZPersistSerializableReferent>(ref));
		mReferentIndexes[ref] = mReferents.size() - 1;
	}
}

void cGZPersistSurveyorWriter::AddDepResource(cIGZPersistResource2* resource)
{
	if (mDependentResourceIndexes.lower_bound(resource) == mDependentResourceIndexes.end())
	{
		mDependentResources.push_back(cRZAutoRefCount<cIGZPersistResource2>(resource));
		mDependentResourceIndexes[resource] = mDependentResources.size() - 1;
	}
}

/**********************************************************
 *
 * NOTE: cGZPersistSurveyorWriter intentionally does not
 * implement any of the primitive Write functions.
 *
 **********************************************************/

void cGZPersistSurveyorWriter::WriteBool(char const* name, bool value) { }
void cGZPersistSurveyorWriter::WriteSint8(char const* name, int8_t value) { }
void cGZPersistSurveyorWriter::WriteUint8(char const* name, uint8_t value) { }
void cGZPersistSurveyorWriter::WriteSint16(char const* name, int16_t value) { }
void cGZPersistSurveyorWriter::WriteUint16(char const* name, uint16_t value) { }
void cGZPersistSurveyorWriter::WriteSint32(char const* name, int32_t value) { }
void cGZPersistSurveyorWriter::WriteUint32(char const* name, uint32_t value) { }
void cGZPersistSurveyorWriter::WriteFloat32(char const* name, float value) { }
void cGZPersistSurveyorWriter::WriteFloat64(char const* name, double value) { }
void cGZPersistSurveyorWriter::WriteBoolArray(char const* name, bool const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteSint8Array(char const* name, int8_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteUint8Array(char const* name, uint8_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteSint16Array(char const* name, int16_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteUint16Array(char const* name, uint16_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteSint32Array(char const* name, int32_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteUint32Array(char const* name, uint32_t const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteFloat32Array(char const* name, float const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteFloat64Array(char const* name, double const* array, size_t length) { }
void cGZPersistSurveyorWriter::WriteString(char const* name, cIGZString const* str) { }
void cGZPersistSurveyorWriter::WriteString(char const* name, char const* str) { }
void cGZPersistSurveyorWriter::WriteTypeInfo(cGZPersistTypeInfo const& info) { }