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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cstdlib>
#include "cGZPersistDBSerialRecord.h"
#include "cRZString.h"

static const size_t StackElementCountLimit = 256;

cGZPersistDBSerialRecord::cGZPersistDBSerialRecord(cIGZPersistDBSegment* segment, bool useLittleEndian) :
	cGZPersistDBRecord(segment),
	cRZOStream(useLittleEndian),
	cRZIStream(useLittleEndian),
	refCount(0),
	key(),
	useLittleEndian(useLittleEndian)
{
}

cGZPersistDBSerialRecord::cGZPersistDBSerialRecord(cGZPersistResourceKey const& key, cIGZPersistDBSegment* segment, bool useLittleEndian) :
	cGZPersistDBRecord(key, segment),
	cRZOStream(useLittleEndian),
	cRZIStream(useLittleEndian),
	refCount(0),
	key(key),
	useLittleEndian(useLittleEndian)
{
}

bool cGZPersistDBSerialRecord::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistDBSerialRecord)
	{
		*outPtr = static_cast<cIGZPersistDBSerialRecord*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZPersistDBRecord)
	{
		*outPtr = static_cast<cIGZPersistDBRecord*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZIStream)
	{
		*outPtr = static_cast<cIGZIStream*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZOStream)
	{
		*outPtr = static_cast<cIGZOStream*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
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

uint32_t cGZPersistDBSerialRecord::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZPersistDBSerialRecord::Release(void)
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

cIGZPersistDBRecord* cGZPersistDBSerialRecord::AsIGZPersistDBRecord(void)
{
	return static_cast<cIGZPersistDBRecord*>(this);
}

bool cGZPersistDBSerialRecord::GetFieldUint8(uint8_t& value)
{
	return GetFieldVoid(&value, 1);
}

bool cGZPersistDBSerialRecord::GetFieldUint8(uint8_t* array, size_t length)
{
	return GetFieldVoid(array, length);
}

bool cGZPersistDBSerialRecord::GetFieldSint8(int8_t& value)
{
	return GetFieldVoid(&value, 1);
}

bool cGZPersistDBSerialRecord::GetFieldSint8(int8_t* array, size_t length)
{
	return GetFieldVoid(array, length);
}

bool cGZPersistDBSerialRecord::GetFieldUint16(uint16_t& value)
{
	uint16_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(uint16_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_ushort(temp);
	}

	value = temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldUint16(uint16_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(uint16_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_ushort(*array);
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint16(int16_t& value)
{
	uint16_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(int16_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_ushort(temp);
	}

	value = (int16_t)temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint16(int16_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(int16_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_ushort(*reinterpret_cast<uint16_t*>(array));
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldUint32(uint32_t& value)
{
	uint32_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(uint32_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_ulong(temp);
	}

	value = temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldUint32(uint32_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(uint32_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_ulong(*array);
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint32(int32_t& value)
{
	uint32_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(int32_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_ulong(temp);
	}

	value = (int32_t)temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint32(int32_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(int32_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_ulong(*reinterpret_cast<int32_t*>(array));
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldUint64(uint64_t& value)
{
	uint64_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(uint64_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_uint64(temp);
	}

	value = temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldUint64(uint64_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(uint64_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_uint64(*array);
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint64(int64_t& value)
{
	uint64_t temp;
	bool succeeded = GetFieldVoid(&temp, sizeof(int64_t));

	if (!useLittleEndian)
	{
		temp = _byteswap_uint64(temp);
	}

	value = (int64_t)temp;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldSint64(int64_t* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(int64_t) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			*array = _byteswap_uint64(*reinterpret_cast<uint64_t*>(array));
			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldFloat32(float& value)
{
	bool succeeded = GetFieldVoid(&value, sizeof(float));

	float readAsFloat = value;
	if (!useLittleEndian)
	{
		uint32_t readAsU32 = *reinterpret_cast<uint32_t*>(&readAsFloat);
		readAsU32 = _byteswap_ulong(readAsU32);
		readAsFloat = *reinterpret_cast<float*>(&readAsU32);
	}

	value = readAsFloat;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldFloat32(float* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(float) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			uint32_t readAsU32 = *reinterpret_cast<uint32_t*>(array);
			readAsU32 = _byteswap_ulong(readAsU32);
			*array = *reinterpret_cast<float*>(&readAsU32);

			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldFloat64(double& value)
{
	bool succeeded = GetFieldVoid(&value, sizeof(float));

	double readAsDouble = value;
	if (!useLittleEndian)
	{
		uint64_t readAsU64 = *reinterpret_cast<uint64_t*>(&readAsDouble);
		readAsU64 = _byteswap_uint64(readAsU64);
		readAsDouble = *reinterpret_cast<double*>(&readAsU64);
	}

	value = readAsDouble;
	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldFloat64(double* array, size_t length)
{
	bool succeeded = GetFieldVoid(array, sizeof(double) * length);
	if (!useLittleEndian && length != 0)
	{
		do
		{
			uint64_t readAsU64 = *reinterpret_cast<uint64_t*>(array);
			readAsU64 = _byteswap_ulong(readAsU64);
			*array = *reinterpret_cast<double*>(&readAsU64);

			array++;
			--length;
		}
		while (length != 0);
	}

	return succeeded;
}

bool cGZPersistDBSerialRecord::GetFieldString(cIGZString& string)
{
	// TODO
	return false;
}

bool cGZPersistDBSerialRecord::GetFieldResKey(cGZPersistResourceKey& key)
{
	return GetFieldUint32(key.type)
		&& GetFieldUint32(key.group)
		&& GetFieldUint32(key.instance);
}

bool cGZPersistDBSerialRecord::GetFieldVariant(cIGZVariant** outPtr)
{
	// TODO
	return false;
}

bool cGZPersistDBSerialRecord::SetFieldUint8(uint8_t value)
{
	return SetFieldVoid(&value, 1);
}

bool cGZPersistDBSerialRecord::SetFieldUint8(uint8_t const* array, size_t length)
{
	return SetFieldVoid(array, length);
}

bool cGZPersistDBSerialRecord::SetFieldSint8(int8_t value)
{
	return SetFieldVoid(&value, 1);
}

bool cGZPersistDBSerialRecord::SetFieldSint8(int8_t const* array, size_t length)
{
	return SetFieldVoid(array, length);
}

bool cGZPersistDBSerialRecord::SetFieldUint16(uint16_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_ushort(value);
	}

	return SetFieldVoid(&value, sizeof(uint16_t));
}

bool cGZPersistDBSerialRecord::SetFieldUint16(uint16_t const* array, size_t length)
{
	// FUTURE: need to check actual platform endianness
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(uint16_t));
	}

	uint16_t stackArray[StackElementCountLimit];
	uint16_t* tempArray;
	uint16_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new uint16_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_ushort(array[i]);
	}

	bool result = SetFieldVoid(array, length * sizeof(uint16_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldSint16(int16_t value)
{
	if (!useLittleEndian)
	{
		value = (int16_t)_byteswap_ushort(value);
	}

	return SetFieldVoid(&value, sizeof(int16_t));
}

bool cGZPersistDBSerialRecord::SetFieldSint16(int16_t const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(int16_t));
	}

	int16_t stackArray[StackElementCountLimit];
	int16_t* tempArray;
	int16_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new int16_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_ushort(*reinterpret_cast<uint16_t const*>(&array[i]));
	}

	bool result = SetFieldVoid(array, length * sizeof(int16_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldUint32(uint32_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_ulong(value);
	}

	return SetFieldVoid(&value, sizeof(uint32_t));
}

bool cGZPersistDBSerialRecord::SetFieldUint32(uint32_t const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(uint32_t));
	}

	uint32_t stackArray[StackElementCountLimit];
	uint32_t* tempArray;
	uint32_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new uint32_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_ulong(array[i]);
	}

	bool result = SetFieldVoid(array, length * sizeof(uint32_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldSint32(int32_t value)
{
	if (!useLittleEndian)
	{
		value = (int32_t)_byteswap_ulong(value);
	}

	return SetFieldVoid(&value, sizeof(int32_t));
}

bool cGZPersistDBSerialRecord::SetFieldSint32(int32_t const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(int32_t));
	}

	int32_t stackArray[StackElementCountLimit];
	int32_t* tempArray;
	int32_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new int32_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_ulong(*reinterpret_cast<uint32_t const*>(&array[i]));
	}

	bool result = SetFieldVoid(array, length * sizeof(int32_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldUint64(uint64_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_uint64(value);
	}

	return SetFieldVoid(&value, sizeof(uint64_t));
}

bool cGZPersistDBSerialRecord::SetFieldUint64(uint64_t const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(uint64_t));
	}

	uint64_t stackArray[StackElementCountLimit];
	uint64_t* tempArray;
	uint64_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new uint64_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_uint64(array[i]);
	}

	bool result = SetFieldVoid(array, length * sizeof(uint64_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldSint64(int64_t value)
{
	if (!useLittleEndian)
	{
		value = (int64_t)_byteswap_uint64(value);
	}

	return SetFieldVoid(&value, sizeof(int64_t));
}

bool cGZPersistDBSerialRecord::SetFieldSint64(int64_t const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(int64_t));
	}

	int64_t stackArray[StackElementCountLimit];
	int64_t* tempArray;
	int64_t* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new int64_t[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		tempArray[i] = _byteswap_uint64(*reinterpret_cast<uint64_t const*>(&array[i]));
	}

	bool result = SetFieldVoid(array, length * sizeof(int64_t));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldFloat32(float value)
{
	if (!useLittleEndian)
	{
		uint32_t asU32 = *reinterpret_cast<uint32_t*>(&value);
		asU32 = _byteswap_ulong(asU32);
		value = *reinterpret_cast<float*>(&asU32);
	}

	return SetFieldVoid(&value, sizeof(float));
}

bool cGZPersistDBSerialRecord::SetFieldFloat32(float const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(float));
	}

	float stackArray[StackElementCountLimit];
	float* tempArray;
	float* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new float[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		uint32_t asU32 = _byteswap_ulong(*reinterpret_cast<uint32_t const*>(&array[i]));
		tempArray[i] = *reinterpret_cast<float*>(&asU32);
	}

	bool result = SetFieldVoid(array, length * sizeof(float));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldFloat64(double value)
{
	if (!useLittleEndian)
	{
		uint64_t asU64 = *reinterpret_cast<uint64_t*>(&value);
		asU64 = _byteswap_ulong(asU64);
		value = *reinterpret_cast<double*>(&asU64);
	}

	return SetFieldVoid(&value, sizeof(double));
}

bool cGZPersistDBSerialRecord::SetFieldFloat64(double const* array, size_t length)
{
	if (useLittleEndian)
	{
		return SetFieldVoid(array, length * sizeof(double));
	}

	double stackArray[StackElementCountLimit];
	double* tempArray;
	double* allocatedArray;

	if (length <= StackElementCountLimit)
	{
		tempArray = stackArray;
		allocatedArray = NULL;
	}
	else
	{
		tempArray = new double[length];
		allocatedArray = tempArray;
	}

	for (size_t i = 0; i < length; i++)
	{
		uint64_t asU64 = _byteswap_uint64(*reinterpret_cast<uint64_t const*>(&array[i]));
		tempArray[i] = *reinterpret_cast<double*>(&asU64);
	}

	bool result = SetFieldVoid(array, length * sizeof(double));
	delete[] allocatedArray;
	return result;
}

bool cGZPersistDBSerialRecord::SetFieldString(cIGZString const& string)
{
	// TODO
	return false;
}

bool cGZPersistDBSerialRecord::SetFieldResKey(cGZPersistResourceKey const& key)
{
	return SetFieldUint32(key.type)
		&& SetFieldUint32(key.group)
		&& SetFieldUint32(key.instance);
}

bool cGZPersistDBSerialRecord::SetFieldVariant(cIGZVariant const* variant)
{
	// TODO
	return false;
}

bool cGZPersistDBSerialRecord::SeekAbsolute(uint32_t position)
{
	return false;
}

bool cGZPersistDBSerialRecord::SeekRelative(int32_t offset)
{
	return false;
}

cGZPersistResourceKey cGZPersistDBSerialRecord::GetKey(void)
{
	return key;
}

bool cGZPersistDBSerialRecord::Skip(uint32_t bytes)
{
	return SeekRelative(bytes);
}

bool cGZPersistDBSerialRecord::GetVoid(void* buffer, uint32_t size)
{
	return GetFieldVoid(buffer, size);
}

bool cGZPersistDBSerialRecord::GetFieldVoid(void* data, uint32_t count, uint32_t elemSize)
{
	return GetFieldVoid(data, count * elemSize);
}

bool cGZPersistDBSerialRecord::SetVoid(void const* buffer, uint32_t size)
{
	return SetFieldVoid(buffer, size);
}

bool cGZPersistDBSerialRecord::SetFieldVoid(void const* data, uint32_t count, uint32_t elemSize)
{
	return SetFieldVoid(data, count * elemSize);
}

bool cGZPersistDBSerialRecord::GetError(void) const
{
	return false;
}

void cGZPersistDBSerialRecord::Flush(void)
{
	return;
}

bool cGZPersistDBSerialRecord::SetRZCharStr(char const* str)
{
	return SetFieldString(cRZString(str));
}

bool cGZPersistDBSerialRecord::SetGZStr(cIGZString const& str)
{
	return SetFieldString(str);
}

cIGZOStream& cGZPersistDBSerialRecord::operator<< (char const* str)
{
	return *static_cast<cIGZOStream*>(this);
}

cIGZOStream& cGZPersistDBSerialRecord::operator<< (cIGZString const& str)
{
	SetFieldString(str);
	return *static_cast<cIGZOStream*>(this);
}

bool cGZPersistDBSerialRecord::GetRZCharStr(char* buffer, uint32_t size)
{
	return false;
}

bool cGZPersistDBSerialRecord::GetGZStr(cIGZString& out)
{
	return GetFieldString(out);
}

cIGZIStream& cGZPersistDBSerialRecord::operator>> (cIGZString& str)
{
	GetFieldString(str);
	return *static_cast<cIGZIStream*>(this);
}