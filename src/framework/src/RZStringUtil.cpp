/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "RZStringUtil.h"

void ConvertStringEncoding(cIGZString const& src, void* dest, uint32_t& destLength, int32_t codePage)
{
	// TODO
}

void ConvertStringEncoding(cIGZString const& src, cIGZString& dest, int32_t codePage)
{
	// TODO
}

void ConvertStringEncoding(cIGZString const& src, int32_t codePage, cIGZString& dest)
{
	// TODO
}

void ConvertStringEncoding(void const* src, uint32_t srcLength, int32_t codePage, cIGZString& dest)
{
	// TODO
}

void ConvertUTF8StringToDestinationFormat(cIGZString& str)
{
	// TODO
}

int32_t GetCharLength(void const* src, uint32_t srcLength, int32_t codePage)
{
	// TODO
	return 0;
}

uint32_t GetCurrentFrameworkCodePage()
{
	return GZ_CODEPAGE_UTF8;
}

void LimitStringLength(cRZString& str, uint32_t maxBytes)
{
	char const* chars = str.begin();
	uint32_t position = 0;
	uint32_t size = str.size();

	if (size == 0)
	{
		return;
	}

	do
	{
		uint32_t charLength = (uint32_t)GetCharLength(chars + position, size - position, GZ_CODEPAGE_NATIVE);
		if (charLength == 0)
		{
			break;
		}

		charLength += position;
		if (charLength > maxBytes)
		{
			break;
		}

		position = charLength;
	}
	while (position < size);

	if (position < size)
	{
		str.erase(position);
	}
}

void Sprintf(cRZString& output, const char* format, ...)
{
	// TODO
}

uint32_t RZAtoUint32HexA(char const*, ...)
{
	// TODO
	return 0;
}

std::vector<wchar_t> RZStringToUTF16(cIGZString const& src)
{
	std::vector<wchar_t> result;
	uint32_t length = src.Strlen();

	if (length > 256)
	{
		std::vector<wchar_t> temp(length);
		ConvertStringEncoding(src, &temp[0], length, GZ_CODEPAGE_UTF16);
		result.assign(temp.begin(), temp.begin() + length);
	}
	else
	{
		wchar_t buffer[256];
		ConvertStringEncoding(src, buffer, length, GZ_CODEPAGE_UTF16);
		result.assign(buffer, buffer + length);
	}

	return result;
}