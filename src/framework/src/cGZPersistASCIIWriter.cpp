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

#include "cGZPersistASCIIWriter.h"
#include "cGZPersistSurveyorWriter.h"
#include "cGZPersistTypeInfo.h"
#include "cIGZCOM.h"
#include "cIGZOStream.h"
#include "cIGZPersistResource2.h"
#include "cIGZPersistSerializableReferent.h"
#include "RZStatics.h"

cGZPersistASCIIWriter::cGZPersistASCIIWriter() :
	mRefCount(0)
{
}

cGZPersistASCIIWriter::cSurveyorList::cSurveyorList()
{
}

cGZPersistASCIIWriter::~cGZPersistASCIIWriter()
{
}

bool cGZPersistASCIIWriter::QueryInterface(GZIID iid, void** outPtr)
{
	if (outPtr == NULL)
	{
		return false;
	}
	else if (iid != GZIID_cIGZUnknown && iid != GZIID_cIGZPersistWriter)
	{
		return false;
	}
	else
	{
		*outPtr = this;
		AddRef();
		return true;
	}
}

uint32_t cGZPersistASCIIWriter::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZPersistASCIIWriter::Release()
{
	if (--mRefCount == 0)
	{
		delete this;
		return 0;
	}

	return mRefCount;
}

void cGZPersistASCIIWriter::WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream)
{
	cRZAutoRefCount<cIGZPersistSurveyorWriter> surveyor;
	GZCOM()->GetClassObject(GZCLSID_cGZPersistSurveyorWriter, GZIID_cIGZPersistSurveyorWriter, reinterpret_cast<void**>(&surveyor));

	mSurveyors.push_back(surveyor);
	mSurveyors.back()->WriteRoot(resource, stream);
	{
		cGZPersistTypeInfo typeInfo = resource->TypeInfo();
		cIGZString const* name = typeInfo.name;
		cIGZOStream* stream = OutputStream();

		stream->SetRZCharStr("Resource ");
		stream->SetGZStr(*name);
		stream->SetRZCharStr(" ");
	}

	WriteKey(resource->AsResource()->GetKey());
	OutputStream()->SetRZCharStr("      . ");

	for (int i = 0; i < mSurveyors.back()->NumDependentResources(); i++)
	{
		cIGZPersistResource2 const* dependent = mSurveyors.back()->DependentResource(i);
		{
			cGZPersistTypeInfo typeInfo = dependent->TypeInfo();
			cIGZString const* name = typeInfo.name;
			cIGZOStream* stream = OutputStream();

			stream->SetRZCharStr("   Dep. Resourcec #");
			stream->SetUint32(i);
			stream->SetRZCharStr(" ");
			stream->SetGZStr(*name);
			stream->SetRZCharStr(" ");
		}

		WriteKey(dependent->AsResource()->GetKey());
		OutputStream()->SetRZCharStr("      . ");
	}

	for (int i = 0; i < mSurveyors.back()->NumObjectRefs(); i++)
	{
		cGZPersistTypeInfo typeInfo = mSurveyors.back()->ObjectRef(i)->TypeInfo();
		cIGZString const* name = typeInfo.name;
		cIGZOStream* stream = OutputStream();

		stream->SetRZCharStr("   Object #");
		stream->SetUint32(i);
		stream->SetRZCharStr(" ");
		stream->SetGZStr(*name);
		stream->SetRZCharStr("      . ");
	}

	OutputStream()->SetRZCharStr("      . ");

	for (int i = 0; i < mSurveyors.back()->NumObjectRefs(); i++)
	{
		cIGZPersistSerializableReferent const* referent = mSurveyors.back()->ObjectRef(i);
		{
			cGZPersistTypeInfo typeInfo = referent->TypeInfo();
			cIGZString const* name = typeInfo.name;
			cIGZOStream* stream = OutputStream();

			stream->SetRZCharStr("   Object #");
			stream->SetUint32(i);
			stream->SetRZCharStr(" ");
			stream->SetGZStr(*name);
			stream->SetRZCharStr("      . ");
		}

		OutputStream()->SetRZCharStr("   {\n");
		referent->WriteTo(this);
		OutputStream()->SetRZCharStr("   }\n\n");
	}

	OutputStream()->SetRZCharStr("      . ");
	OutputStream()->Flush();

	for (int i = 0; i < mSurveyors.back()->NumDependentResources(); i++)
	{
		mSurveyors.back()->DependentResource(i)->Saved();
	}

	mSurveyors.pop_back();
}

void cGZPersistASCIIWriter::WriteBool(char const* name, bool value)
{
	char const* text = value ? "true" : false;
	cIGZOStream* stream = OutputStream();

	stream->SetRZCharStr("      bool ");
	stream->SetRZCharStr(name);
	stream->SetRZCharStr(" = ");
	stream->SetRZCharStr(text);
	stream->SetRZCharStr("      . ");
}

#define DEF_PRIMITIVE_VALUE_WRITER(typeName, type) \
	void cGZPersistASCIIWriter::Write ## typeName ## (char const* name, type value) \
	{ \
		cIGZOStream* stream = OutputStream(); \
		stream->SetRZCharStr("      " ## #typeName ## " "); \
		stream->SetRZCharStr(name); \
		stream->SetRZCharStr(" = "); \
		stream->Set ## typeName ## (value); \
		stream->SetRZCharStr("      . "); \
	}

DEF_PRIMITIVE_VALUE_WRITER(Sint8, int8_t);
DEF_PRIMITIVE_VALUE_WRITER(Uint8, uint8_t);
DEF_PRIMITIVE_VALUE_WRITER(Sint16, int16_t);
DEF_PRIMITIVE_VALUE_WRITER(Uint16, uint16_t);
DEF_PRIMITIVE_VALUE_WRITER(Sint32, int32_t);
DEF_PRIMITIVE_VALUE_WRITER(Uint32, uint32_t);
DEF_PRIMITIVE_VALUE_WRITER(Float32, float);
DEF_PRIMITIVE_VALUE_WRITER(Float64, double);

void cGZPersistASCIIWriter::WriteBoolArray(char const* name, bool const* array, size_t length)
{
	cIGZOStream* stream = OutputStream(); \
	stream->SetRZCharStr("      bool ");
	stream->SetRZCharStr(name);
	stream->SetRZCharStr("[");
	stream->SetUint32((uint32_t)length);
	stream->SetRZCharStr("] = ");

	for (int i = 0; i < length; i++)
	{
		char const* text = *array++ ? "true" : "false";
		cIGZOStream* innerStream = OutputStream();

		innerStream->SetRZCharStr(text);
		innerStream->SetRZCharStr(", ");
	}

	OutputStream()->SetRZCharStr("      . ");
}

#define DEF_PRIMITIVE_ARRAY_WRITER(typeName, type) \
	void cGZPersistASCIIWriter::Write ## typeName ## Array(char const* name, type const* array, size_t length) \
	{ \
		cIGZOStream* stream = OutputStream(); \
		stream->SetRZCharStr("      " ## #typeName ## " "); \
		stream->SetRZCharStr(name); \
		stream->SetRZCharStr("["); \
		stream->SetUint32((uint32_t)length); \
		stream->SetRZCharStr("] = "); \
		\
		for (int i = 0; i < length; i++) \
		{ \
			type value = *array; \
			cIGZOStream* innerStream = OutputStream(); \
			innerStream->Set ## typeName ## (value); \
			innerStream->SetRZCharStr(", "); \
			array++; \
		} \
		\
		OutputStream()->SetRZCharStr("      . "); \
	}

DEF_PRIMITIVE_ARRAY_WRITER(Sint8, int8_t);
DEF_PRIMITIVE_ARRAY_WRITER(Uint8, uint8_t);
DEF_PRIMITIVE_ARRAY_WRITER(Sint16, int16_t);
DEF_PRIMITIVE_ARRAY_WRITER(Uint16, uint16_t);
DEF_PRIMITIVE_ARRAY_WRITER(Sint32, int32_t);
DEF_PRIMITIVE_ARRAY_WRITER(Uint32, uint32_t);
DEF_PRIMITIVE_ARRAY_WRITER(Float32, float);
DEF_PRIMITIVE_ARRAY_WRITER(Float64, double);

void cGZPersistASCIIWriter::WriteString(char const* name, cIGZString const* str)
{
	cIGZOStream* stream = OutputStream();
	stream->SetRZCharStr("      RZString ");
	stream->SetRZCharStr(name);
	stream->SetRZCharStr(" = ");
	stream->SetGZStr(*str);
	stream->SetRZCharStr("      . ");
}

void cGZPersistASCIIWriter::WriteString(char const* name, char const* str)
{
	cIGZOStream* stream = OutputStream();
	stream->SetRZCharStr("      RZChar* ");
	stream->SetRZCharStr(name);
	stream->SetRZCharStr(" = ");
	stream->SetRZCharStr(str);
	stream->SetRZCharStr("      . ");
}

void cGZPersistASCIIWriter::WriteObject(char const* name, cIGZPersistSerializable2 const* obj)
{
	OutputStream()->SetRZCharStr("      Object GZCLSID: ");

	if (obj != NULL)
	{
		WriteValue(obj->TypeInfo().clsid);

		OutputStream()->SetRZCharStr("\n      {\n");
		obj->WriteTo(this);
		OutputStream()->SetRZCharStr("      }\n");
	}
	else
	{
		WriteValue(0);
	}

	OutputStream()->SetRZCharStr("      . ");
}

void cGZPersistASCIIWriter::WriteObjectRef(char const* name, cIGZPersistSerializableReferent const* obj)
{
	cIGZOStream* stream = OutputStream();
	stream->SetRZCharStr("      cIGZPersistSerializableReferent* ");
	stream->SetRZCharStr(name);
	stream->SetRZCharStr(" = ");

	if (obj != NULL)
	{
		cIGZPersistResource2* resource = obj->Resource();

		if (mSurveyors.back()->Resource() != resource)
		{
			cIGZPersistSurveyorWriter* surveyor = mSurveyors.back();

			cGZPersistTypeInfo typeInfo = obj->TypeInfo();
			cIGZString const* typeName = typeInfo.name;
			cIGZOStream* innerStream = OutputStream();

			innerStream->SetRZCharStr("Object ID ");
			uint32_t objIndex = resource->IndexOfObject(obj);
			innerStream->SetUint32(objIndex);

			innerStream->SetRZCharStr(" ");
			innerStream->SetGZStr(*typeName);

			innerStream->SetRZCharStr(" in dep. resource #");
			uint32_t depIndex = surveyor->IndexOfDependentResource(resource);
			innerStream->SetUint32(depIndex);

			innerStream->SetRZCharStr(" ");
			WriteKey(resource->AsResource()->GetKey());
		}
		else
		{
			cIGZPersistSurveyorWriter* surveyor = mSurveyors.back();

			cGZPersistTypeInfo typeInfo = obj->TypeInfo();
			cIGZString const* typeName = typeInfo.name;
			cIGZOStream* innerStream = OutputStream();

			innerStream->SetRZCharStr("Object #");
			uint32_t objIndex = surveyor->IndexOfObjectRef(obj);
			innerStream->SetUint16(objIndex);

			innerStream->SetRZCharStr(" ");
			innerStream->SetGZStr(*typeName);

			OutputStream()->SetRZCharStr(" GZCLSID: ");
			WriteValue(obj->TypeInfo().clsid);
		}
	}
	else
	{
		OutputStream()->SetRZCharStr("0");
	}

	OutputStream()->SetRZCharStr("      . ");
}

void cGZPersistASCIIWriter::WriteObjectRef(char const* name, cIGZUnknown const* obj)
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

void cGZPersistASCIIWriter::WriteTypeInfo(cGZPersistTypeInfo const& info)
{
	cIGZString const* name = info.name;
	cIGZOStream* stream = OutputStream();

	stream->SetRZCharStr("      TypeName: ");
	stream->SetGZStr(*name);
	stream->SetRZCharStr(" GZCLSID: ");
	WriteValue(info.clsid);

	stream = OutputStream();
	stream->SetRZCharStr(" Version: ");
	stream->SetUint32(info.data);
	stream->SetRZCharStr("      . ");
}

cIGZOStream* cGZPersistASCIIWriter::OutputStream()
{
	return mSurveyors.back()->OutputStream();
}

void cGZPersistASCIIWriter::WriteKey(cGZPersistResourceKey const& key)
{
	OutputStream()->SetRZCharStr("G:");
	WriteValue(key.group);

	OutputStream()->SetRZCharStr(" I:");
	WriteValue(key.instance);

	OutputStream()->SetRZCharStr(" T:");
	WriteValue(key.type);
}

void cGZPersistASCIIWriter::WriteValue(uint32_t value)
{
	char buffer[] = "0x00000000";
	sprintf(buffer, "0x%08x", value);
	OutputStream()->SetRZCharStr(buffer);
}