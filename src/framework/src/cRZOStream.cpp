/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include <cstring>
#include "cIGZSerializable.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cRZOStream.h"

cRZOStream::cRZOStream(bool useLittleEndian)
{
	this->useLittleEndian = useLittleEndian;
}

cRZOStream::~cRZOStream()
{
	if (userData != NULL)
	{
		userData->Release();
	}
}

bool cRZOStream::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid = GZIID_cIGZOStream)
	{
		*outPtr = static_cast<cIGZOStream*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZOStream::AddRef()
{
	return ++refCount;
}

uint32_t cRZOStream::Release()
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

bool cRZOStream::SetSint8(int8_t value)
{
	int8_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint8(uint8_t value)
{
	uint8_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetSint16(int16_t value)
{
	// FUTURE: See cRZIStream, this should change depending on compiler target
	if (!useLittleEndian)
	{
		value = (int16_t)_byteswap_ushort((uint16_t)value);
	}

	int16_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint16(uint16_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_ushort(value);
	}

	uint16_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetSint32(int32_t value)
{
	if (!useLittleEndian)
	{
		value = (int32_t)_byteswap_ulong((uint32_t)value);
	}

	int32_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint32(uint32_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_ulong(value);
	}

	uint32_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetSint64(int64_t value)
{
	if (!useLittleEndian)
	{
		value = (int64_t)_byteswap_uint64((uint64_t)value);
	}

	int64_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint64(uint64_t value)
{
	if (!useLittleEndian)
	{
		value = _byteswap_uint64(value);
	}

	uint64_t data = value;
	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetFloat32(float value)
{
	double storedValue = value;
	uint32_t dataAsInt = *reinterpret_cast<uint32_t*>(&storedValue);

	if (!useLittleEndian)
	{
		dataAsInt = _byteswap_ulong(dataAsInt);
	}

	return SetVoid(&dataAsInt, sizeof(dataAsInt));
}

bool cRZOStream::SetFloat64(double value)
{
	double storedValue = value;
	uint64_t dataAsInt = *reinterpret_cast<uint64_t*>(&storedValue);

	if (!useLittleEndian)
	{
		dataAsInt = _byteswap_uint64(dataAsInt);
	}

	return SetVoid(&dataAsInt, sizeof(dataAsInt));
}

bool cRZOStream::SetRZCharStr(char const* str)
{
	uint32_t length = strlen(str);
	EncodeStringLength(length);

	bool result = true;
	if (length != 0)
	{
		result = SetVoid(str, length);
	}

	return result;
}

bool cRZOStream::SetGZStr(cIGZString const& str)
{
	uint32_t length = str.Strlen();
	EncodeStringLength(length);

	bool result = true;
	if (length != 0)
	{
		result = SetVoid(str.ToChar(), length);
	}

	return result;
}

bool cRZOStream::SetGZSerializable(cIGZSerializable const& data)
{
	return data.Write(*this);
}

void cRZOStream::SetUserData(cIGZVariant* data)
{
	cIGZVariant* savedData = this->userData;
	if (savedData != data)
	{
		if (data != NULL)
		{
			data->AddRef();
		}

		this->userData = data;

		if (savedData != NULL)
		{
			savedData->Release();
		}
	}
}

cIGZVariant* cRZOStream::GetUserData()
{
	return userData;
}

bool cRZOStream::Init(bool useLittleEndian)
{
	this->useLittleEndian = useLittleEndian;
	return true;
}

bool cRZOStream::Shutdown()
{
	return true;
}

void cRZOStream::EncodeStringLength(uint32_t length)
{
	if (length == 0)
	{
		SetUint8(0);
	}
	else
	{
		uint8_t byte;
		while (true)
		{
			byte = (uint8_t)length;
			length >>= 7;

			if (length == 0)
			{
				break;
			}

			SetUint8(byte | 0x80);
		}

		SetUint8(byte & 0x7F);
	}
}