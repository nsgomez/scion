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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <cstring>
#include "cIGZSerializable.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cRZOStream.h"

cRZOStream::cRZOStream() :
	mRefCount(0),
	mUserData(NULL)
{
	mUseLittleEndian = false;
}

cRZOStream::cRZOStream(bool useLittleEndian) :
	mRefCount(0),
	mUserData(NULL)
{
	mUseLittleEndian = useLittleEndian;
}

cRZOStream::~cRZOStream()
{
}

bool cRZOStream::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZOStream:
		*outPtr = static_cast<cIGZOStream*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	default:
		return false;
	}
}

uint32_t cRZOStream::AddRef()
{
	return ++mRefCount;
}

uint32_t cRZOStream::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cRZOStream::SetSint8(int8_t value)
{
	return SetVoid(&value, sizeof(value));
}

bool cRZOStream::SetUint8(uint8_t value)
{
	return SetVoid(&value, sizeof(value));
}

bool cRZOStream::SetSint16(int16_t value)
{
	// FUTURE: See cRZIStream, this should change depending on compiler target
	int16_t data;
	if (mUseLittleEndian)
	{
		data = value;
	}
	else
	{
		volatile uint32_t swapped = (uint16_t)value;
		swapped = _byteswap_ulong(swapped);
		data = (int16_t)(swapped >> 16);
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint16(uint16_t value)
{
	uint16_t data;
	if (mUseLittleEndian)
	{
		data = value;
	}
	else
	{
		volatile uint32_t swapped = value;
		swapped = _byteswap_ulong(swapped);
		data = (uint16_t)(swapped >> 16);
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetSint32(int32_t value)
{
	int32_t data = value;
	if (!mUseLittleEndian)
	{
		volatile uint32_t swapped = (uint32_t)value;
		swapped = _byteswap_ulong(swapped);
		data = (int32_t)swapped;
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint32(uint32_t value)
{
	uint32_t data = value;
	if (!mUseLittleEndian)
	{
		volatile uint32_t swapped = value;
		swapped = _byteswap_ulong(swapped);
		data = swapped;
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetSint64(int64_t value)
{
	int64_t data;
	if (mUseLittleEndian)
	{
		data = value;
	}
	else
	{
		value = (int64_t)_byteswap_uint64((uint64_t)value);
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetUint64(uint64_t value)
{
	uint64_t data;
	if (mUseLittleEndian)
	{
		data = value;
	}
	else
	{
		value = _byteswap_uint64(value);
	}

	return SetVoid(&data, sizeof(data));
}

bool cRZOStream::SetFloat32(float value)
{
	return SetVoid(&value, sizeof(value));
}

bool cRZOStream::SetFloat64(double value)
{
	return SetVoid(&value, sizeof(value));
}

bool cRZOStream::SetRZCharStr(char const* str)
{
	uint32_t length = strlen(str);
	EncodeStringLength(length);

	if (length > 0)
	{
		return SetVoid(str, length);
	}

	return true;
}

bool cRZOStream::SetGZStr(cIGZString const& str)
{
	uint32_t length = str.Strlen();
	EncodeStringLength(length);

	if (length != 0)
	{
		return SetVoid(str.ToChar(), length);
	}

	return true;
}

bool cRZOStream::SetGZSerializable(cIGZSerializable const& data)
{
	return data.Write(*this);
}

void cRZOStream::SetUserData(cIGZVariant* data)
{
	mUserData = data;
}

cIGZVariant* cRZOStream::GetUserData()
{
	return mUserData;
}

bool cRZOStream::Init(bool useLittleEndian)
{
	mUseLittleEndian = useLittleEndian;
	return true;
}

bool cRZOStream::Shutdown()
{
	return true;
}

void cRZOStream::EncodeStringLength(uint32_t length)
{
	if (length != 0)
	{
		uint8_t byte = (uint8_t)(length & 0x7f);
		uint32_t remaining = length >> 7;

		while (remaining != 0)
		{
			SetUint8(byte | 0x80);
			byte = (uint8_t)(remaining & 0x7f);
			remaining >>= 7;
		}

		SetUint8(byte);
	}
	else
	{
		SetUint8(0);
	}
}