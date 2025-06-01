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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cIGZString.h"
#include "cRZOFStream.h"

cRZOFStream::cRZOFStream(cIGZString const& path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append) : cRZTextOStream(useLittleEndian)
{
	outFile = new std::ofstream(path.ToChar(), append ? (std::ios_base::out | std::ios_base::app) : (std::ios_base::out | std::ios_base::trunc));
	this->binary = binary;
	this->alwaysFlushStrings = alwaysFlushStrings;
}

cRZOFStream::cRZOFStream(char const* path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append) : cRZTextOStream(useLittleEndian)
{
	outFile = new std::ofstream(path, append ? (std::ios_base::out | std::ios_base::app) : (std::ios_base::out | std::ios_base::trunc));
	this->binary = binary;
	this->alwaysFlushStrings = alwaysFlushStrings;
}

cRZOFStream::~cRZOFStream()
{
	if (outFile != NULL)
	{
		delete outFile;
	}
}

void cRZOFStream::Flush(void)
{
	outFile->flush();
}

bool cRZOFStream::SetSint8(int8_t value)
{
	return !binary ? cRZTextOStream::SetSint8(value) : cRZOStream::SetSint8(value);
}

bool cRZOFStream::SetUint8(uint8_t value)
{
	return !binary ? cRZTextOStream::SetUint8(value) : cRZOStream::SetUint8(value);
}

bool cRZOFStream::SetSint16(int16_t value)
{
	return !binary ? cRZTextOStream::SetSint16(value) : cRZOStream::SetSint16(value);
}

bool cRZOFStream::SetUint16(uint16_t value)
{
	return !binary ? cRZTextOStream::SetUint16(value) : cRZOStream::SetUint16(value);
}

bool cRZOFStream::SetSint32(int32_t value)
{
	return !binary ? cRZTextOStream::SetSint32(value) : cRZOStream::SetSint32(value);
}

bool cRZOFStream::SetUint32(uint32_t value)
{
	return !binary ? cRZTextOStream::SetUint32(value) : cRZOStream::SetUint32(value);
}

bool cRZOFStream::SetSint64(int64_t value)
{
	return !binary ? cRZTextOStream::SetSint64(value) : cRZOStream::SetSint64(value);
}

bool cRZOFStream::SetUint64(uint64_t value)
{
	return !binary ? cRZTextOStream::SetUint64(value) : cRZOStream::SetUint64(value);
}

bool cRZOFStream::SetFloat32(float value)
{
	return !binary ? cRZTextOStream::SetFloat32(value) : cRZOStream::SetFloat32(value);
}

bool cRZOFStream::SetFloat64(double value)
{
	return !binary ? cRZTextOStream::SetFloat64(value) : cRZOStream::SetFloat64(value);
}

bool cRZOFStream::SetRZCharStr(char const* str)
{
	bool success;
	if (!binary)
	{
		if (str == NULL)
		{
			outFile->clear();
		}
		else
		{
			(*outFile) << str;
		}

		success = outFile->good();
	}
	else
	{
		success = SetVoid(str, strlen(str));
	}

	if (alwaysFlushStrings)
	{
		Flush();
	}

	return success;
}

bool cRZOFStream::SetVoid(void const* buffer, uint32_t size)
{
	outFile->write(static_cast<char const*>(buffer), size);
	return outFile->good();
}

bool cRZOFStream::GetError(void) const
{
	return outFile->good();
}