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
#include "cGZPersistTypeInfo.h"
#include "cIGZOStream.h"
#include "cIGZPersistSerializable2.h"
#include "cIGZPersistSerializableReferent.h"

cGZPersistBinaryWriter::cGZPersistBinaryWriter(void) :
	refCount(0)
{
}

bool cGZPersistBinaryWriter::QueryInterface(GZIID iid, void** outPtr)
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
	else if (iid == GZIID_cIGZPersistWriter)
	{
		*outPtr = static_cast<cIGZPersistWriter*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistBinaryWriter::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZPersistBinaryWriter::Release(void)
{
	if (--refCount == 0)
	{
		delete this;
	}

	return refCount;
}

void cGZPersistBinaryWriter::WriteRoot(cIGZPersistResource2 const* resource, cIGZOStream* stream)
{
	// TODO
}

void cGZPersistBinaryWriter::WriteBool(bool value)
{
	OutputStream()->SetUint8(value);
}

void cGZPersistBinaryWriter::WriteSint8(int8_t value)
{
	OutputStream()->SetSint8(value);
}

void cGZPersistBinaryWriter::WriteUint8(uint8_t value)
{
	OutputStream()->SetUint8(value);
}

void cGZPersistBinaryWriter::WriteSint16(int16_t value)
{
	OutputStream()->SetSint16(value);
}

void cGZPersistBinaryWriter::WriteUint16(uint16_t value)
{
	OutputStream()->SetUint16(value);
}

void cGZPersistBinaryWriter::WriteSint32(int32_t value)
{
	OutputStream()->SetSint32(value);
}

void cGZPersistBinaryWriter::WriteUint32(uint32_t value)
{
	OutputStream()->SetUint32(value);
}

void cGZPersistBinaryWriter::WriteFloat32(float value)
{
	OutputStream()->SetFloat32(value);
}

void cGZPersistBinaryWriter::WriteFloat64(double value)
{
	OutputStream()->SetFloat64(value);
}

void cGZPersistBinaryWriter::WriteBoolArray(bool const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint8(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteSint8Array(int8_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetSint8(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteUint8Array(uint8_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint8(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteSint16Array(int16_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetSint16(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteUint16Array(uint16_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint16(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteSint32Array(int32_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetSint32(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteUint32Array(uint32_t const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetUint32(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteFloat32Array(float const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetFloat32(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteFloat64Array(double const* array, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		OutputStream()->SetFloat64(*(array++));
	}
}

void cGZPersistBinaryWriter::WriteString(cIGZString const* str)
{
	OutputStream()->SetGZStr(*str);
}

void cGZPersistBinaryWriter::WriteString(char const* str)
{
	OutputStream()->SetRZCharStr(str);
}

void cGZPersistBinaryWriter::WriteObject(cIGZPersistSerializable2 const* obj)
{
	if (obj == NULL)
	{
		OutputStream()->SetUint32(0);
	}
	else
	{
		OutputStream()->SetUint32(obj->TypeInfo().clsid);
		obj->WriteTo(this);
	}
}

void cGZPersistBinaryWriter::WriteObjectRef(cIGZPersistSerializableReferent const* obj)
{
	// TODO
}

void cGZPersistBinaryWriter::WriteObjectRef(cIGZUnknown const* obj)
{
	cIGZPersistSerializableReferent* referent = NULL;
	if (obj != NULL)
	{
		const_cast<cIGZUnknown*>(obj)->QueryInterface(GZIID_cIGZPersistSerializableReferent, reinterpret_cast<void**>(&referent));
	}

	WriteObjectRef(referent);

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
	// TODO
	return NULL;
}
