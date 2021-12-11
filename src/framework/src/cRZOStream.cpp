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

#include "cRZOStream.h"

cRZOStream::cRZOStream(bool useLittleEndian)
{
}

cRZOStream::~cRZOStream()
{
}

bool cRZOStream::SetSint8(int8_t value)
{
}

bool cRZOStream::SetUint8(uint8_t value)
{
}

bool cRZOStream::SetSint16(int16_t value)
{
}

bool cRZOStream::SetUint16(uint16_t value)
{
}

bool cRZOStream::SetSint32(int32_t value)
{
}

bool cRZOStream::SetUint32(uint32_t value)
{
}

bool cRZOStream::SetSint64(int64_t value)
{
}

bool cRZOStream::SetUint64(uint64_t value)
{
}

bool cRZOStream::SetFloat32(float value)
{
}

bool cRZOStream::SetFloat64(double value)
{
}

bool cRZOStream::SetRZCharStr(char const* str)
{
}

bool cRZOStream::SetGZStr(cIGZString const& str)
{
}

bool cRZOStream::SetGZSerializable(cIGZSerializable const& data)
{
}

void cRZOStream::SetUserData(cIGZVariant* data)
{
}

cIGZVariant* cRZOStream::GetUserData()
{
}

bool cRZOStream::Init(bool useLittleEndian)
{
}

bool cRZOStream::Shutdown()
{
}

void cRZOStream::EncodeStringLength(uint32_t length)
{
}