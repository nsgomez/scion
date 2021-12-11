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

#include "cRZIStream.h"

cRZIStream::cRZIStream(bool useLittleEndian)
{
}

cRZIStream::~cRZIStream()
{
}

bool cRZIStream::GetSint8(int8_t& out)
{
}

bool cRZIStream::GetUint8(uint8_t& out)
{
}

bool cRZIStream::GetSint16(int16_t& out)
{
}

bool cRZIStream::GetUint16(uint16_t& out)
{
}

bool cRZIStream::GetSint32(int32_t& out)
{
}

bool cRZIStream::GetUint32(uint32_t& out)
{
}

bool cRZIStream::GetSint64(int64_t& out)
{
}

bool cRZIStream::GetUint64(uint64_t& out)
{
}

bool cRZIStream::GetFloat32(float& out)
{
}

bool cRZIStream::GetFloat64(double& out)
{
}

bool cRZIStream::GetRZCharStr(char const* buffer, uint32_t size)
{
}

bool cRZIStream::GetGZStr(cIGZString& out)
{
}

bool cRZIStream::GetGZSerializable(cIGZSerializable& out)
{
}

void cRZIStream::SetUserData(cIGZVariant* data)
{
}

cIGZVariant* cRZIStream::GetUserData()
{
}

bool cRZIStream::Init(bool useLittleEndian)
{
}

bool cRZIStream::Shutdown()
{
}

void cRZIStream::DecodeStringLength(uint32_t& length)
{
}