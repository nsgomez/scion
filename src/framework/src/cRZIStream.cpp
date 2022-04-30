/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include <cstdlib>
#include "cIGZSerializable.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cRZIStream.h"

cRZIStream::cRZIStream(bool useLittleEndian)
{
	this->useLittleEndian = useLittleEndian;
}

cRZIStream::~cRZIStream()
{
	if (userData != NULL)
	{
		userData->Release();
	}
}

bool cRZIStream::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid = GZIID_cIGZIStream)
	{
		*outPtr = static_cast<cIGZIStream*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZIStream::AddRef()
{
	return ++refCount;
}

uint32_t cRZIStream::Release()
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

bool cRZIStream::GetSint8(int8_t& out)
{
	return GetVoid(&out, sizeof(out));
}

bool cRZIStream::GetUint8(uint8_t& out)
{
	return GetVoid(&out, sizeof(out));
}

bool cRZIStream::GetSint16(int16_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		// TODO: During compilation we should check the target endianness and do the byte swaps accordingly.
		if (!useLittleEndian)
		{
			out = (int16_t)_byteswap_ushort((uint16_t)out);
		}

		return true;
	}
}

bool cRZIStream::GetUint16(uint16_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			out = _byteswap_ushort(out);
		}

		return true;
	}
}

bool cRZIStream::GetSint32(int32_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			out = (int32_t)_byteswap_ulong((uint32_t)out);
		}

		return true;
	}
}

bool cRZIStream::GetUint32(uint32_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			out = _byteswap_ulong(out);
		}

		return true;
	}
}

bool cRZIStream::GetSint64(int64_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			out = (int64_t)_byteswap_uint64((uint64_t)out);
		}

		return true;
	}
}

bool cRZIStream::GetUint64(uint64_t& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			out = _byteswap_uint64(out);
		}

		return true;
	}
}

bool cRZIStream::GetFloat32(float& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			uint32_t dataAsInt = *reinterpret_cast<uint32_t*>(&out);
			dataAsInt = _byteswap_ulong(dataAsInt);
			out = *reinterpret_cast<float*>(&dataAsInt);
		}

		return true;
	}
}

bool cRZIStream::GetFloat64(double& out)
{
	if (!GetVoid(&out, sizeof(out)))
	{
		return false;
	}
	else
	{
		if (!useLittleEndian)
		{
			uint64_t dataAsInt = *reinterpret_cast<uint64_t*>(&out);
			dataAsInt = _byteswap_uint64(dataAsInt);
			out = *reinterpret_cast<double*>(&dataAsInt);
		}

		return true;
	}
}

bool cRZIStream::GetRZCharStr(char* buffer, uint32_t size)
{
	uint32_t decodedLength;
	DecodeStringLength(decodedLength);

	if (decodedLength == 0 || GetError())
	{
		buffer[0] = '\0';
	}
	else
	{
		if (decodedLength < size)
		{
			size = decodedLength;
		}

		GetVoid(buffer, size);
		buffer[size] = '\0';

		if (!GetError() && (decodedLength - size) != 0)
		{
			Skip(decodedLength - size);
		}
	}

	return !GetError();
}

bool cRZIStream::GetGZStr(cIGZString& out)
{
	uint32_t totalLength = 0;
	uint32_t chunkSize;
	char buffer[256];

	out.FromChar("");
	DecodeStringLength(totalLength);

	if (!GetError())
	{
		do
		{
			chunkSize = totalLength;
			if (chunkSize > 255)
			{
				chunkSize = 255;
			}

			GetVoid(buffer, chunkSize);
			buffer[chunkSize] = 0;
			out.Append(buffer, chunkSize);

			totalLength -= chunkSize;
		}
		while (totalLength > 0 && !GetError());
	}

	return !GetError();
}

bool cRZIStream::GetGZSerializable(cIGZSerializable& out)
{
	return out.Read(*this);
}

void cRZIStream::SetUserData(cIGZVariant* data)
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

cIGZVariant* cRZIStream::GetUserData()
{
	return userData;
}

bool cRZIStream::Init(bool useLittleEndian)
{
	this->useLittleEndian = useLittleEndian;
	return true;
}

bool cRZIStream::Shutdown()
{
	return true;
}

void cRZIStream::DecodeStringLength(uint32_t& length)
{
	uint8_t currentByte;
	uint8_t lengthBytes[4];

	length = 0;
	GetUint8(currentByte);

	if (currentByte != 0)
	{
		int additionalByteCount = 0;
		while (currentByte > 0x7F)
		{
			lengthBytes[additionalByteCount] = currentByte & 0x7F;

			GetUint8(lengthBytes[additionalByteCount + 1]);
			currentByte = lengthBytes[additionalByteCount + 1];

			++additionalByteCount;
		}

		do
		{
			length = lengthBytes[additionalByteCount] | (length << 7);
		}
		while (--additionalByteCount >= 0);
	}
}