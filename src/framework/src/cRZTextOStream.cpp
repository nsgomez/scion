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

#include "cIGZString.h"
#include "cRZString.h"
#include "cRZTextOStream.h"

static char const* kFmtSint   = "%d";
static char const* kFmtUint   = "%u";
static char const* kFmtSint64 = "%lld";
static char const* kFmtUint64 = "%llu";
static char const* kFmtFloat  = "%g";

bool cRZTextOStream::SetSint8(int8_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtSint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetUint8(uint8_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtUint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetSint16(int16_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtSint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetUint16(uint16_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtUint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetSint32(int32_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtSint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetUint32(uint32_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtUint, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetSint64(int64_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtSint64, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetUint64(uint64_t value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtUint64, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetFloat32(float value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtFloat, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetFloat64(double value)
{
	cRZString tmp;
	tmp.Sprintf(kFmtFloat, value);
	return SetRZCharStr(tmp.Data());
}

bool cRZTextOStream::SetGZStr(cIGZString const& str)
{
	return SetRZCharStr(str.ToChar());
}

bool cRZTextOStream::SetVoid(void const* buffer, uint32_t size)
{
	char line[256];
	bool ok = SetRZCharStr("\nHEXDUMP");

	char const* bytes = static_cast<char const*>(buffer);
	for (size_t i = 0; i < size && ok; i++)
	{
		sprintf(line, "%02x ", static_cast<int32_t>(bytes[i]));
		ok = SetRZCharStr(line);

		if ((i + 1) % 25 == 0)
		{
			ok = ok && SetRZCharStr("\n");
		}
	}

	return ok && SetRZCharStr("\n");
}

bool cRZTextOStream::GetError() const
{
	return true;
}