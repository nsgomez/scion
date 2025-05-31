/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cGZPersistBinaryReader.h"
#include "cIGZCOM.h"
#include "cIGZPersistSerializable2.h"
#include "cIGZIStream.h"
#include "cRZString.h"
#include "RZStatics.h"

cGZPersistBinaryReader::cGZPersistBinaryReader(void) :
	refCount(0),
	resources()
{
}

bool cGZPersistBinaryReader::QueryInterface(GZIID iid, void** outPtr)
{
	if (outPtr == NULL)
	{
		return false;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZPersistReader)
	{
		*outPtr = static_cast<cIGZPersistReader*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistBinaryReader::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZPersistBinaryReader::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
	}

	return refCount;
}

void cGZPersistBinaryReader::ReadRoot(cIGZPersistResource2* resource, cIGZIStream* stream)
{
	// TODO
}

bool cGZPersistBinaryReader::ReadBool(void)
{
	uint8_t data;
	InputStream()->GetUint8(data);
	return !!data;
}

int8_t cGZPersistBinaryReader::ReadSint8(void)
{
	int8_t data;
	InputStream()->GetSint8(data);
	return data;
}

uint8_t cGZPersistBinaryReader::ReadUint8(void)
{
	uint8_t data;
	InputStream()->GetUint8(data);
	return data;
}

int16_t cGZPersistBinaryReader::ReadSint16(void)
{
	int16_t data;
	InputStream()->GetSint16(data);
	return data;
}

uint16_t cGZPersistBinaryReader::ReadUint16(void)
{
	uint16_t data;
	InputStream()->GetUint16(data);
	return data;
}

int32_t cGZPersistBinaryReader::ReadSint32(void)
{
	int32_t data;
	InputStream()->GetSint32(data);
	return data;
}

uint32_t cGZPersistBinaryReader::ReadUint32(void)
{
	uint32_t data;
	InputStream()->GetUint32(data);
	return data;
}

float cGZPersistBinaryReader::ReadFloat32(void)
{
	float data;
	InputStream()->GetFloat32(data);
	return data;
}

double cGZPersistBinaryReader::ReadFloat64(void)
{
	double data;
	InputStream()->GetFloat64(data);
	return data;
}

void cGZPersistBinaryReader::ReadBoolArray(bool* array, size_t length)
{
	uint8_t data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetUint8(data);
		*(array++) = !!data;
	}
}

void cGZPersistBinaryReader::ReadSint8Array(int8_t* array, size_t length)
{
	InputStream()->GetVoid(array, length);
}

void cGZPersistBinaryReader::ReadUint8Array(uint8_t* array, size_t length)
{
	InputStream()->GetVoid(array, length);
}

void cGZPersistBinaryReader::ReadSint16Array(int16_t* array, size_t length)
{
	// FUTURE: why not use GetVoid here?
	int16_t data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetSint16(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadUint16Array(uint16_t* array, size_t length)
{
	uint16_t data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetUint16(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadSint32Array(int32_t* array, size_t length)
{
	int32_t data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetSint32(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadUint32Array(uint32_t* array, size_t length)
{
	uint32_t data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetUint32(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadFloat32Array(float* array, size_t length)
{
	float data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetFloat32(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadFloat64Array(double* array, size_t length)
{
	double data;
	for (size_t i = 0; i < length; i++)
	{
		InputStream()->GetFloat64(data);
		*(array++) = data;
	}
}

void cGZPersistBinaryReader::ReadString(cIGZString* str)
{
	InputStream()->GetGZStr(*str);
}

void cGZPersistBinaryReader::SkipBytes(size_t length)
{
	InputStream()->Skip(length);
}

void cGZPersistBinaryReader::ReadObject(GZIID iid, void** out)
{
	GZCLSID clsid;
	cIGZPersistSerializable2* serializable;
	InputStream()->GetUint32(clsid);

	if (!clsid)
	{
		*out = NULL;
	}
	else
	{
		GZCOM()->GetClassObject(clsid, GZIID_cIGZPersistSerializable2, reinterpret_cast<void**>(&serializable));
		serializable->ReadFrom(this);
		serializable->QueryInterface(iid, out);

		// FUTURE: this check should've happened earlier
		if (serializable != NULL)
		{
			serializable->Release();
		}
	}
}

void cGZPersistBinaryReader::ReadObjectRef(GZCLSID clsid, void** out)
{
	// TODO
}

cGZPersistTypeInfo cGZPersistBinaryReader::ReadTypeInfo(void)
{
	cGZPersistTypeInfo info;
	cIGZString* name = new cRZString();

	info.name = name;

	InputStream()->GetGZStr(*info.name);
	InputStream()->GetUint32(info.clsid);
	InputStream()->GetUint32(info.data);

	return info;
}

cIGZIStream* cGZPersistBinaryReader::InputStream(void)
{
	// TODO
	return NULL;
}