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

#include "cGZPersistBinaryReader.h"
#include "cGZPersistResourceManager.h"
#include "cIGZCOM.h"
#include "cIGZPersistSerializable2.h"
#include "cIGZIStream.h"
#include "cRZString.h"
#include "cRZSysServPtr.h"
#include "RZStatics.h"

cGZPersistBinaryReader::cResourceInfo::cResourceInfo(cIGZIStream* stream) :
	dependents(),
	objectRefs(),
	stream(stream)
{
}

cGZPersistBinaryReader::cResourceInfoList::cResourceInfoList()
{
}

cGZPersistBinaryReader::cGZPersistBinaryReader(void) :
	mRefCount(0)
{
}

bool cGZPersistBinaryReader::QueryInterface(GZIID iid, void** outPtr)
{
	if (outPtr == NULL)
	{
		return false;
	}

	switch (iid)
	{
	case GZIID_cIGZPersistReader:
		*outPtr = static_cast<cIGZPersistReader*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZPersistBinaryReader::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cGZPersistBinaryReader::Release(void)
{
	if (--mRefCount == 0)
	{
		delete this;
		return 0;
	}

	return mRefCount;
}

void cGZPersistBinaryReader::ReadRoot(cIGZPersistResource2* resource, cIGZIStream* stream)
{
	mResources.push_back(cResourceInfo(stream));

	cRZSysServPtr<cIGZPersistResourceManager, GZIID_cIGZPersistResourceManager, GZCLSID_cGZPersistResourceManager> manager;

	bool resourceWasPreRegistered = manager->HasRegisteredResource(resource->AsResource()->GetKey());
	if (!resourceWasPreRegistered)
	{
		manager->RegisterResource(resource->AsResource()->GetKey(), resource->AsResource());
	}

	uint32_t numDependentResources;
	InputStream()->GetUint32(numDependentResources);

	for (uint32_t i = 0; i < numDependentResources; i++)
	{
		cGZPersistResourceKey key;
		InputStream()->GetUint32(key.group);
		InputStream()->GetUint32(key.instance);
		InputStream()->GetUint32(key.type);

		cRZAutoRefCount<cIGZPersistResource2> dependent;
		if (key.type != 0)
		{
			if (!manager->GetResource(key, GZIID_cIGZPersistResource2, reinterpret_cast<void**>(&dependent), 0, NULL))
			{
				cRZString message;
				message.Sprintf("Cannot load dependent resource with group ID = %u, instance ID = %u, type = %u\n",
					key.group, key.instance, key.type);
			}
		}
		else
		{
			cRZString message;
			message.Sprintf("Null dependent resource with group ID = %u, instance ID = %u, type = %u\n",
				key.group, key.instance, key.type);
		}

		mResources.back().dependents.push_back(dependent);
	}

	uint32_t numObjectRefs;
	InputStream()->GetUint32(numObjectRefs);

	mResources.back().objectRefs.push_back(cRZAutoRefCount<cIGZPersistSerializableReferent>(resource));

	uint32_t clsid;
	InputStream()->GetUint32(clsid);

	for (uint32_t i = 0; i < numObjectRefs; i++)
	{
		InputStream()->GetUint32(clsid);

		if (clsid != 0)
		{
			cRZAutoRefCount<cIGZPersistSerializableReferent> obj;
			cIGZCOM* com = GZCOM();

			obj = NULL;
			com->GetClassObject(clsid, GZIID_cIGZPersistSerializableReferent, reinterpret_cast<void**>(&obj));

			mResources.back().objectRefs.push_back(obj);
		}
		else
		{
			mResources.back().objectRefs.push_back(cRZAutoRefCount<cIGZPersistSerializableReferent>());
		}
	}

	for (uint32_t i = 0; i < numObjectRefs; i++)
	{
		if (mResources.back().objectRefs[i] != NULL)
		{
			mResources.back().objectRefs[i]->ReadFrom(this);
		}
	}

	if (!resourceWasPreRegistered)
	{
		manager->UnregisterResource(resource->AsResource()->GetKey());
	}

	mResources.pop_back();
}

bool cGZPersistBinaryReader::ReadBool(char const* name)
{
	uint8_t data;
	InputStream()->GetUint8(data);
	return !!data;
}

int8_t cGZPersistBinaryReader::ReadSint8(char const* name)
{
	int8_t data;
	InputStream()->GetSint8(data);
	return data;
}

uint8_t cGZPersistBinaryReader::ReadUint8(char const* name)
{
	uint8_t data;
	InputStream()->GetUint8(data);
	return data;
}

int16_t cGZPersistBinaryReader::ReadSint16(char const* name)
{
	int16_t data;
	InputStream()->GetSint16(data);
	return data;
}

uint16_t cGZPersistBinaryReader::ReadUint16(char const* name)
{
	uint16_t data;
	InputStream()->GetUint16(data);
	return data;
}

int32_t cGZPersistBinaryReader::ReadSint32(char const* name)
{
	int32_t data;
	InputStream()->GetSint32(data);
	return data;
}

uint32_t cGZPersistBinaryReader::ReadUint32(char const* name)
{
	uint32_t data;
	InputStream()->GetUint32(data);
	return data;
}

float cGZPersistBinaryReader::ReadFloat32(char const* name)
{
	float data;
	InputStream()->GetFloat32(data);
	return data;
}

double cGZPersistBinaryReader::ReadFloat64(char const* name)
{
	double data;
	InputStream()->GetFloat64(data);
	return data;
}

void cGZPersistBinaryReader::ReadBoolArray(char const* name, bool* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		uint8_t data;
		InputStream()->GetUint8(data);
		*(array++) = !!data;
	}
}

void cGZPersistBinaryReader::ReadSint8Array(char const* name, int8_t* array, size_t length)
{
	InputStream()->GetVoid(array, length);
}

void cGZPersistBinaryReader::ReadUint8Array(char const* name, uint8_t* array, size_t length)
{
	InputStream()->GetVoid(array, length);
}

void cGZPersistBinaryReader::ReadSint16Array(char const* name, int16_t* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetSint16(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadUint16Array(char const* name, uint16_t* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetUint16(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadSint32Array(char const* name, int32_t* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetSint32(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadUint32Array(char const* name, uint32_t* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetUint32(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadFloat32Array(char const* name, float* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetFloat32(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadFloat64Array(char const* name, double* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetFloat64(*array);
		array++;
	}
}

void cGZPersistBinaryReader::ReadString(char const* name, cIGZString* str)
{
	InputStream()->GetGZStr(*str);
}

void cGZPersistBinaryReader::SkipBytes(char const* name, size_t length)
{
	InputStream()->Skip(length);
}

void cGZPersistBinaryReader::ReadObject(char const* name, GZIID iid, void** out)
{
	GZCLSID clsid;
	InputStream()->GetUint32(clsid);

	if (clsid != 0)
	{
		cIGZPersistSerializable2* serializable = NULL;
		GZCOM()->GetClassObject(clsid, GZIID_cIGZPersistSerializable2, reinterpret_cast<void**>(&serializable));

		serializable->ReadFrom(this);
		serializable->QueryInterface(iid, out);

		// FUTURE: this check should've happened earlier
		if (serializable != NULL)
		{
			serializable->Release();
		}
	}
	else
	{
		*out = NULL;
	}
}

void cGZPersistBinaryReader::ReadObjectRef(char const* name, GZIID iid, void** out)
{
	uint8_t hasRef;
	InputStream()->GetUint8(hasRef);

	if (hasRef)
	{
		uint8_t isDependentResource;
		uint32_t index;

		InputStream()->GetUint8(isDependentResource);
		InputStream()->GetUint32(index);

		*out = NULL;

		if (isDependentResource)
		{
			if (mResources.back().dependents[index] != NULL)
			{
				mResources.back().dependents[index]->QueryInterface(iid, out);
			}
		}
		else
		{
			if (mResources.back().objectRefs[index] != NULL)
			{
				mResources.back().objectRefs[index]->QueryInterface(iid, out);
			}
		}
	}
	else
	{
		*out = NULL;
	}
}

cGZPersistTypeInfo cGZPersistBinaryReader::ReadTypeInfo(void)
{
	cGZPersistTypeInfo info;
	info.name = new cRZString();

	InputStream()->GetGZStr(*info.name);
	InputStream()->GetUint32(info.clsid);
	InputStream()->GetUint32(info.data);

	return info;
}

cIGZIStream* cGZPersistBinaryReader::InputStream(void)
{
	return mResources.back().stream;
}