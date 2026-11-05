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
#include "cRZOFStream.h"

cRZOFStream::cRZOFStream(cIGZString const& path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append) : cRZTextOStream(true)
{
	mFile = new std::ofstream(path.ToChar(),
		(append ? (std::ios_base::out | std::ios_base::app) : (std::ios_base::out | std::ios_base::trunc))
		| (binary ? std::ios_base::binary : 0));

	mBinary = binary;
	mAlwaysFlushStrings = mAlwaysFlushStrings;
}

cRZOFStream::cRZOFStream(char const* path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append) : cRZTextOStream(true)
{
	mFile = new std::ofstream(path,
		(append ? (std::ios_base::out | std::ios_base::app) : (std::ios_base::out | std::ios_base::trunc))
		| (binary ? std::ios_base::binary : 0));

	mBinary = binary;
	mAlwaysFlushStrings = alwaysFlushStrings;
}

cRZOFStream::~cRZOFStream()
{
	if (mFile != NULL)
	{
		delete mFile;
	}
}

void cRZOFStream::Flush(void)
{
	mFile->flush();
}

bool cRZOFStream::SetSint8(int8_t value)
{
	return mBinary ? cRZOStream::SetSint8(value) : cRZTextOStream::SetSint8(value);
}

bool cRZOFStream::SetUint8(uint8_t value)
{
	return mBinary ? cRZOStream::SetUint8(value) : cRZTextOStream::SetUint8(value);
}

bool cRZOFStream::SetSint16(int16_t value)
{
	return mBinary ? cRZOStream::SetSint16(value) : cRZTextOStream::SetSint16(value);
}

bool cRZOFStream::SetUint16(uint16_t value)
{
	return mBinary ? cRZOStream::SetUint16(value) : cRZTextOStream::SetUint16(value);
}

bool cRZOFStream::SetSint32(int32_t value)
{
	return mBinary ? cRZOStream::SetSint32(value) : cRZTextOStream::SetSint32(value);
}

bool cRZOFStream::SetUint32(uint32_t value)
{
	return mBinary ? cRZOStream::SetUint32(value) : cRZTextOStream::SetUint32(value);
}

bool cRZOFStream::SetSint64(int64_t value)
{
	return mBinary ? cRZOStream::SetSint64(value) : cRZTextOStream::SetSint64(value);
}

bool cRZOFStream::SetUint64(uint64_t value)
{
	return mBinary ? cRZOStream::SetUint64(value) : cRZTextOStream::SetUint64(value);
}

bool cRZOFStream::SetFloat32(float value)
{
	return mBinary ? cRZOStream::SetFloat32(value) : cRZTextOStream::SetFloat32(value);
}

bool cRZOFStream::SetFloat64(double value)
{
	return mBinary ? cRZOStream::SetFloat64(value) : cRZTextOStream::SetFloat64(value);
}

bool cRZOFStream::SetRZCharStr(char const* str)
{
	bool success;
	if (mBinary)
	{
		success = SetVoid(str, strlen(str));
	}
	else
	{
		(*mFile) << str;
		success = mFile->good();
	}

	if (mAlwaysFlushStrings)
	{
		Flush();
	}

	return success;
}

bool cRZOFStream::SetVoid(void const* buffer, uint32_t size)
{
	mFile->write(static_cast<char const*>(buffer), size);
	return mFile->good();
}

bool cRZOFStream::GetError(void) const
{
	return mFile->good();
}