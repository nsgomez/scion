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

#include "cIGZString.h"
#include "cRZString.h"
#include "cRZTextOStream.h"

bool cRZTextOStream::SetSint8(int8_t value)
{
	cRZString tmp;
	tmp.Sprintf("%d", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetUint8(uint8_t value)
{
	cRZString tmp;
	tmp.Sprintf("%u", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetSint16(int16_t value)
{
	cRZString tmp;
	tmp.Sprintf("%d", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetUint16(uint16_t value)
{
	cRZString tmp;
	tmp.Sprintf("%u", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetSint32(int32_t value)
{
	cRZString tmp;
	tmp.Sprintf("%d", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetUint32(uint32_t value)
{
	cRZString tmp;
	tmp.Sprintf("%u", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetSint64(int64_t value)
{
	cRZString tmp;
	tmp.Sprintf("%11d", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetUint64(uint64_t value)
{
	cRZString tmp;
	tmp.Sprintf("%11u", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetFloat32(float value)
{
	cRZString tmp;
	tmp.Sprintf("%g", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetFloat64(double value)
{
	cRZString tmp;
	tmp.Sprintf("%g", value);
	return SetRZCharStr(tmp.ToChar());
}

bool cRZTextOStream::SetGZStr(cIGZString const& str)
{
	return SetRZCharStr(str.ToChar());
}