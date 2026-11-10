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

#include "cGZPersistBinaryWriter.h"
#include "cGZPersistSurveyorWriter.h"
#include "cGZPersistTypeInfo.h"
#include "cIGZCOM.h"
#include "cIGZOStream.h"
#include "cIGZPersistResource.h"
#include "cIGZPersistResource2.h"
#include "cIGZPersistSerializable2.h"
#include "cIGZPersistSerializableReferent.h"
#include "RZStatics.h"

cGZPersistBinaryWriter::cSurveyorList::cSurveyorList()
{
}

cGZPersistBinaryWriter::cGZPersistBinaryWriter(void) :
	mRefCount(0)
{
}

bool cGZPersistBinaryWriter::QueryInterface(GZIID iid, void** outPtr)
{
	if (outPtr == NULL)
	{
		return false;
	}

	switch (iid)
	{
	case GZIID_cIGZPersistWriter:
		*outPtr = static_cast<cIGZPersistWriter*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZPersistBinaryWriter::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cGZPersistBinaryWriter::Release(void)
{
	if (--mRefCount == 0)
	{
		delete this;
		return 0;
	}

	return mRefCount;
}

void cGZPersistBinaryWriter::WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream)
{
	cRZAutoRefCount<cIGZPersistSurveyorWriter> surveyor;
	GZCOM()->GetClassObject(GZCLSID_cGZPersistSurveyorWriter, GZIID_cIGZPersistSurveyorWriter, reinterpret_cast<void**>(&surveyor));

	mSurveyors.push_back(surveyor);
	mSurveyors.back()->WriteRoot(resource, stream);

	int32_t numDependentResources = mSurveyors.back()->NumDependentResources();
	OutputStream()->SetUint32(numDependentResources);

	for (int32_t i = 0; i < mSurveyors.back()->NumDependentResources(); i++)
	{
		cIGZPersistResource2 const* dependent = mSurveyors.back()->DependentResource(i);

		OutputStream()->SetUint32(dependent->AsResource()->GetKey().group);
		OutputStream()->SetUint32(dependent->AsResource()->GetKey().instance);
		OutputStream()->SetUint32(dependent->AsResource()->GetKey().type);
	}

	int32_t numObjectRefs = mSurveyors.back()->NumObjectRefs();
	OutputStream()->SetUint32(numObjectRefs);

	for (int32_t i = 0; i < mSurveyors.back()->NumObjectRefs(); i++)
	{
		OutputStream()->SetUint32(mSurveyors.back()->ObjectRef(i)->TypeInfo().clsid);
	}

	for (int32_t i = 0; i < mSurveyors.back()->NumObjectRefs(); i++)
	{
		mSurveyors.back()->ObjectRef(i)->WriteTo(this);
	}

	OutputStream()->Flush();

	for (int32_t i = 0; i < mSurveyors.back()->NumDependentResources(); i++)
	{
		mSurveyors.back()->DependentResource(i)->Saved();
	}

	mSurveyors.pop_back();
}

void cGZPersistBinaryWriter::WriteBool(char const* name, bool value)
{
	OutputStream()->SetUint8(value);
}

void cGZPersistBinaryWriter::WriteSint8(char const* name, int8_t value)
{
	OutputStream()->SetSint8(value);
}

void cGZPersistBinaryWriter::WriteUint8(char const* name, uint8_t value)
{
	OutputStream()->SetUint8(value);
}

void cGZPersistBinaryWriter::WriteSint16(char const* name, int16_t value)
{
	OutputStream()->SetSint16(value);
}

void cGZPersistBinaryWriter::WriteUint16(char const* name, uint16_t value)
{
	OutputStream()->SetUint16(value);
}

void cGZPersistBinaryWriter::WriteSint32(char const* name, int32_t value)
{
	OutputStream()->SetSint32(value);
}

void cGZPersistBinaryWriter::WriteUint32(char const* name, uint32_t value)
{
	OutputStream()->SetUint32(value);
}

void cGZPersistBinaryWriter::WriteFloat32(char const* name, float value)
{
	OutputStream()->SetFloat32(value);
}

void cGZPersistBinaryWriter::WriteFloat64(char const* name, double value)
{
	OutputStream()->SetFloat64(value);
}

void cGZPersistBinaryWriter::WriteBoolArray(char const* name, bool const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint8(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteSint8Array(char const* name, int8_t const* array, size_t length)
{
	OutputStream()->SetVoid(array, length);
}

void cGZPersistBinaryWriter::WriteUint8Array(char const* name, uint8_t const* array, size_t length)
{
	OutputStream()->SetVoid(array, length);
}

void cGZPersistBinaryWriter::WriteSint16Array(char const* name, int16_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetSint16(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteUint16Array(char const* name, uint16_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint16(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteSint32Array(char const* name, int32_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetSint32(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteUint32Array(char const* name, uint32_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint32(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteFloat32Array(char const* name, float const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetFloat32(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteFloat64Array(char const* name, double const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetFloat64(*array);
		array++;
	}
}

void cGZPersistBinaryWriter::WriteString(char const* name, cIGZString const* str)
{
	OutputStream()->SetGZStr(*str);
}

void cGZPersistBinaryWriter::WriteString(char const* name, char const* str)
{
	OutputStream()->SetRZCharStr(str);
}

void cGZPersistBinaryWriter::WriteObject(char const* name, cIGZPersistSerializable2 const* obj)
{
	if (obj != NULL)
	{
		OutputStream()->SetUint32(obj->TypeInfo().clsid);
		obj->WriteTo(this);
	}
	else
	{
		OutputStream()->SetUint32(0);
	}
}

void cGZPersistBinaryWriter::WriteObjectRef(char const* name, cIGZPersistSerializableReferent const* obj)
{
	OutputStream()->SetUint8(obj != NULL);
	if (obj != NULL)
	{
		cIGZPersistResource2* resource = obj->Resource();
		OutputStream()->SetUint8(mSurveyors.back()->Resource() != resource);

		uint32_t index;
		if (mSurveyors.back()->Resource() != resource)
		{
			index = mSurveyors.back()->IndexOfDependentResource(resource);
		}
		else
		{
			index = mSurveyors.back()->IndexOfObjectRef(obj);
		}

		OutputStream()->SetUint32(index);
	}
}

void cGZPersistBinaryWriter::WriteObjectRef(char const* name, cIGZUnknown const* obj)
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

void cGZPersistBinaryWriter::WriteTypeInfo(cGZPersistTypeInfo const& info)
{
	OutputStream()->SetGZStr(*info.name);
	OutputStream()->SetUint32(info.clsid);
	OutputStream()->SetUint32(info.data);
}

cIGZOStream* cGZPersistBinaryWriter::OutputStream()
{
	return mSurveyors.back()->OutputStream();
}
