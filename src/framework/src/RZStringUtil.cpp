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

#include <clocale>
#include <cstdlib>
#include <cstring>
#include <Windows.h>
#include "cIGZString.h"
#include "cRZString.h"
#include "RZStringUtil.h"

static uint32_t ConvertStringEncoding_via_UTF16(
	void const* src,
	uint32_t srcLength,
	GZCodePage srcCodePage,
	void* dest,
	uint32_t& destLength,
	GZCodePage destCodePage)
{
	uint32_t bufferLength = srcLength * 2 + 4;

	char stackBuffer[1024];
	std::vector<char> heapBuffer;

	char* buffer = stackBuffer;
	if (bufferLength > sizeof(stackBuffer))
	{
		heapBuffer.resize(bufferLength);
		buffer = &heapBuffer[0];
	}

	uint32_t convertedLength = ConvertStringEncoding(src, srcLength, srcCodePage, buffer, bufferLength, kGZCodePageUTF16);
	if (convertedLength == srcLength)
	{
		convertedLength = ConvertStringEncoding(buffer, bufferLength, kGZCodePageUTF16, dest, destLength, destCodePage);
		if (convertedLength == bufferLength)
		{
			return srcLength;
		}
	}

	destLength = 0;
	return 0;
}

uint32_t ConvertStringEncoding(
	void const* src,
	uint32_t srcLength,
	GZCodePage srcCodePage,
	void* dest,
	uint32_t& destLength,
	GZCodePage destCodePage)
{
	if (srcLength == 0)
	{
		destLength = 0;
		return 0;
	}

	if (srcCodePage == kGZCodePageNative)
	{
		srcCodePage = kGZCodePageUTF8;
	}
	else if (srcCodePage == kGZCodePageSystem)
	{
		srcCodePage = GetCurrentSystemCodePage();
	}

	if (destCodePage == kGZCodePageNative)
	{
		destCodePage = kGZCodePageUTF8;
	}
	else if (destCodePage == kGZCodePageSystem)
	{
		destCodePage = GetCurrentSystemCodePage();
	}

	if (srcCodePage != kGZCodePageUTF16)
	{
		unsigned char const* scanPtr = static_cast<unsigned char const*>(src);
		bool isAscii = true;

		for (uint32_t i = 0; i < srcLength; i++)
		{
			if (scanPtr[i] > 0x7F)
			{
				isAscii = false;
				break;
			}
		}

		if (isAscii)
		{
			srcCodePage = kGZCodePageANSI;
		}
	}

	if (srcCodePage == destCodePage
		|| (srcCodePage == kGZCodePageANSI && destCodePage != kGZCodePageUTF16)
		|| (srcCodePage != kGZCodePageUTF16 && destCodePage == kGZCodePageANSI))
	{
		uint32_t charsConverted = srcLength;
		if (src != dest)
		{
			if (destLength < charsConverted)
			{
				charsConverted = destLength;
			}

			memmove(dest, src, destCodePage == kGZCodePageUTF16 ? charsConverted * 2 : charsConverted);
		}

		destLength = charsConverted;
		return charsConverted;
	}

	if (srcCodePage == kGZCodePageUTF16 && destCodePage == kGZCodePageANSI)
	{
		uint32_t charsConverted = srcLength;
		if (destLength < charsConverted)
		{
			charsConverted = destLength;
		}

		wchar_t const* utf16Src = static_cast<wchar_t const*>(src);
		unsigned char* ansiDest = static_cast<unsigned char*>(dest);

		for (uint32_t i = 0; i < charsConverted; ++i)
		{
			ansiDest[i] = (unsigned char)utf16Src[i];
		}

		destLength = charsConverted;
		return charsConverted;
	}

	if (src == dest)
	{
		return 0;
	}

	if (srcCodePage == kGZCodePageUTF8)
	{
		if (destCodePage != kGZCodePageUTF16)
		{
			return ConvertStringEncoding_via_UTF16(src, srcLength, srcCodePage, dest, destLength, destCodePage);
		}

		// UTF8 -> UTF16
		unsigned char const* utf8It = static_cast<unsigned char const*>(src);
		unsigned char const* utf8End = utf8It + srcLength;

		wchar_t* utf16It = static_cast<wchar_t*>(dest);
		wchar_t* utf16End = utf16It + destLength;

		while (utf8It < utf8End && utf16It < utf16End)
		{
			unsigned char lead = *utf8It;
			wchar_t charResult;

			if (lead < 0x80) // No continuation byte
			{
				charResult = lead;
				utf8It++;
			}
			else if (lead < 0xc2) // Invalid multi-byte sequence
			{
				break;
			}
			else if (lead < 0xe0) // Two-byte sequence
			{
				if (utf8End - utf8It < 2)
				{
					break;
				}

				unsigned char continuation = (unsigned char)(utf8It[1] ^ 0x80);
				if (continuation >= 0x40)
				{
					break;
				}

				charResult = lead & 0x1F;
				charResult = (wchar_t)((charResult << 6) | continuation);
				utf8It += 2;
			}
			else if (lead < 0xf0) // Three-byte sequence
			{
				if (utf8End - utf8It < 3)
				{
					break;
				}

				unsigned char cont1 = (unsigned char)(utf8It[1] ^ 0x80);
				if (cont1 >= 0x40)
				{
					break;
				}

				unsigned char cont2 = (unsigned char)(utf8It[2] ^ 0x80);
				if (cont2 >= 0x40)
				{
					break;
				}

				if (lead < 0xe1 && utf8It[1] < 0xa0)
				{
					// Reject over-long UTF-8 string
					break;
				}

				charResult = lead;
				charResult = (wchar_t)((charResult << 6) | cont1);
				charResult = (wchar_t)((charResult << 6) | cont2);
				utf8It += 3;
			}
			else if (lead < 0xf8) // Four-byte sequence
			{
				if (utf8End - utf8It < 4)
				{
					break;
				}

				unsigned char cont1 = (unsigned char)(utf8It[1] ^ 0x80);
				if (cont1 >= 0x40)
				{
					break;
				}

				unsigned char cont2 = (unsigned char)(utf8It[2] ^ 0x80);
				if (cont2 >= 0x40)
				{
					break;
				}

				unsigned char cont3 = (unsigned char)(utf8It[3] ^ 0x80);
				if (cont3 >= 0x40)
				{
					break;
				}

				if (lead < 0xf1 && utf8It[1] < 0x90)
				{
					// Reject over-long UTF-8 string
					break;
				}

				charResult = lead;
				charResult = (wchar_t)((charResult << 6) | cont1);
				charResult = (wchar_t)((charResult << 6) | cont2);
				charResult = (wchar_t)((charResult << 6) | cont3);
				utf8It += 4;
			}
			else if (lead < 0xfc) // Five-byte sequence
			{
				if (utf8End - utf8It < 5)
				{
					break;
				}

				unsigned char cont1 = (unsigned char)(utf8It[1] ^ 0x80);
				if (cont1 >= 0x40)
				{
					break;
				}

				unsigned char cont2 = (unsigned char)(utf8It[2] ^ 0x80);
				if (cont2 >= 0x40)
				{
					break;
				}

				unsigned char cont3 = (unsigned char)(utf8It[3] ^ 0x80);
				if (cont3 >= 0x40)
				{
					break;
				}

				unsigned char cont4 = (unsigned char)(utf8It[4] ^ 0x80);
				if (cont4 >= 0x40)
				{
					break;
				}

				if (lead < 0xf9 && utf8It[1] < 0x88)
				{
					// Reject over-long UTF-8 string
					break;
				}

				charResult = lead;
				charResult = (wchar_t)((charResult << 6) | cont1);
				charResult = (wchar_t)((charResult << 6) | cont2);
				charResult = (wchar_t)((charResult << 6) | cont3);
				charResult = (wchar_t)((charResult << 6) | cont4);
				utf8It += 5;
			}
			else if (lead < 0xfe) // Six-byte sequence
			{
				if (utf8End - utf8It < 6)
				{
					break;
				}

				unsigned char cont1 = (unsigned char)(utf8It[1] ^ 0x80);
				if (cont1 >= 0x40)
				{
					break;
				}

				unsigned char cont2 = (unsigned char)(utf8It[2] ^ 0x80);
				if (cont2 >= 0x40)
				{
					break;
				}

				unsigned char cont3 = (unsigned char)(utf8It[3] ^ 0x80);
				if (cont3 >= 0x40)
				{
					break;
				}

				unsigned char cont4 = (unsigned char)(utf8It[4] ^ 0x80);
				if (cont4 >= 0x40)
				{
					break;
				}

				unsigned char cont5 = (unsigned char)(utf8It[5] ^ 0x80);
				if (cont5 >= 0x40)
				{
					break;
				}

				if (lead < 0xfd && utf8It[1] < 0x84)
				{
					// Reject over-long UTF-8 string
					break;
				}

				charResult = lead;
				charResult = (wchar_t)((charResult << 6) | cont1);
				charResult = (wchar_t)((charResult << 6) | cont2);
				charResult = (wchar_t)((charResult << 6) | cont3);
				charResult = (wchar_t)((charResult << 6) | cont4);
				charResult = (wchar_t)((charResult << 6) | cont5);
				utf8It += 6;
			}
			else // Invalid multi-byte sequence
			{
				break;
			}

			*utf16It++ = charResult;
		}

		destLength = (uint32_t)(utf16It - static_cast<uint16_t*>(dest));
		return (uint32_t)(utf8It - static_cast<unsigned char const*>(src));
	}

	if (srcCodePage == kGZCodePageUTF16)
	{
		if (destCodePage != kGZCodePageUTF8)
		{
			if (destCodePage == kGZCodePageUTF16)
			{
				destLength = 0;
				return 0;
			}

			BOOL usedDefaultChar = FALSE;
			char defaultChar = ' ';
			DWORD convertedLength = WideCharToMultiByte(
				(UINT)destCodePage,
				0,
				static_cast<LPCWCH>(src),
				srcLength,
				static_cast<LPSTR>(dest),
				destLength,
				&defaultChar,
				&usedDefaultChar);

			if (convertedLength != 0)
			{
				destLength = convertedLength;
				return srcLength;
			}

			destLength = 0;
			return 0;
		}

		// UTF16 -> UTF8
		wchar_t const* utf16It = static_cast<wchar_t const*>(src);
		wchar_t const* utf16End = utf16It + srcLength;

		unsigned char* utf8It = static_cast<unsigned char*>(dest);
		unsigned char* utf8End = utf8It + destLength;

		while (utf16It < utf16End)
		{
			uint32_t ch = *utf16It++;
			uint32_t charsNeeded;

			if (ch < 0x80)
			{
				charsNeeded = 1;
			}
			else if (ch < 0x800)
			{
				charsNeeded = 2;
			}
			else if (ch < 0x10000)
			{
				charsNeeded = 3;
			}
			else if (ch < 0x200000)
			{
				charsNeeded = 4;
			}
			else if (ch < 0x4000000)
			{
				charsNeeded = 5;
			}
			else if (ch <= 0x7fffffff)
			{
				charsNeeded = 6;
			}
			else
			{
				break;
			}

			if (utf8It + charsNeeded > utf8End)
			{
				break;
			}

			switch (charsNeeded)
			{
			case 6:
				utf8It[5] = (unsigned char)((ch & 0x3f) | 0x80);
				ch = (ch >> 6) | 0x4000000;

			case 5:
				ch |= 0x8000000;
				utf8It[4] = (unsigned char)((ch & 0x3f) | 0x80);
				ch >>= 6;

			case 4:
				ch |= 0x400000;
				utf8It[3] = (unsigned char)((ch & 0x3f) | 0x80);
				ch >>= 6;

			case 3:
				ch |= 0x20000;
				utf8It[2] = (unsigned char)((ch & 0x3f) | 0x80);
				ch >>= 6;

			case 2:
				ch |= 0x3000;
				utf8It[1] = (unsigned char)((ch & 0x3f) | 0x80);
				ch >>= 6;

			case 1:
				utf8It[0] = (unsigned char)ch;
			}

			utf8It += charsNeeded;
			if (utf16It >= utf16End)
			{
				break;
			}
		}

		destLength = (uint32_t)(utf8It - static_cast<unsigned char*>(dest));
		return (uint32_t)(utf16It - static_cast<wchar_t const*>(src));
	}

	if (srcCodePage == kGZCodePageANSI && destCodePage == kGZCodePageUTF16)
	{
		uint32_t charsConverted = srcLength;
		if (destLength < charsConverted)
		{
			charsConverted = destLength;
		}

		unsigned char const* utf8It = static_cast<unsigned char const*>(src);
		wchar_t* utf16It = static_cast<wchar_t*>(dest);

		for (uint32_t i = 0; i < charsConverted; i++)
		{
			*utf16It++ = utf8It[i];
		}

		destLength = charsConverted;
		return charsConverted;
	}

	if (destCodePage == kGZCodePageUTF8)
	{
		return ConvertStringEncoding_via_UTF16(src, srcLength, srcCodePage, dest, destLength, destCodePage);
	}

	if (destCodePage != kGZCodePageUTF16)
	{
		destLength = 0;
		return 0;
	}

	int32_t charsConverted = MultiByteToWideChar(srcCodePage, 0, static_cast<LPCCH>(src), srcLength, static_cast<LPWSTR>(dest), destLength);
	if (charsConverted != 0)
	{
		destLength = charsConverted;
		return srcLength;
	}

	destLength = 0;
	return 0;
}

uint32_t ConvertStringEncoding(void const* src, uint32_t srcLength, GZCodePage codePage, cIGZString& dest)
{
	uint32_t charsConverted;
	if (codePage == kGZCodePageNative)
	{
		goto utf8;
	}

	if (codePage == kGZCodePageSystem)
	{
		codePage = GetCurrentSystemCodePage();
	}

	if (codePage == kGZCodePageUTF8)
	{
utf8:
		dest.FromChar(static_cast<char const*>(src), srcLength);
		charsConverted = srcLength;
	}
	else
	{
		char* heapBuffer;
		uint32_t bufferLength = srcLength * 6;
		char stackBuffer[512];

		if (bufferLength < sizeof(stackBuffer))
		{
			heapBuffer = NULL;
		}
		else
		{
			heapBuffer = new char[bufferLength];
		}

		char* buffer = heapBuffer;
		if (buffer == NULL)
		{
			buffer = stackBuffer;
		}

		buffer[0] = '\0';
		charsConverted = ConvertStringEncoding(src, srcLength, codePage, buffer, bufferLength, kGZCodePageUTF8);

		dest.FromChar(buffer, bufferLength);
		delete[] heapBuffer;
	}

	return charsConverted;
}

uint32_t ConvertStringEncoding(cIGZString const& src, void* dest, uint32_t& destLength, GZCodePage codePage)
{
	uint32_t charsConverted;
	if (codePage == kGZCodePageNative)
	{
		goto utf8;
	}

	if (codePage == kGZCodePageSystem)
	{
		codePage = GetCurrentSystemCodePage();
	}

	if (codePage == kGZCodePageUTF8)
	{
utf8:
		if (destLength > src.Strlen())
		{
			destLength = src.Strlen();
		}

		memmove(dest, src.Data(), destLength);
		charsConverted = destLength;
	}
	else
	{
		charsConverted = ConvertStringEncoding(src.Data(), src.Strlen(), kGZCodePageUTF8, dest, destLength, codePage);
	}

	return charsConverted;
}

bool ConvertStringEncoding(cIGZString const& src, GZCodePage srcCodePage, cIGZString& dest)
{
	bool result = false;
	if (srcCodePage == kGZCodePageNative)
	{
		goto utf8;
	}

	if (srcCodePage == kGZCodePageSystem)
	{
		srcCodePage = GetCurrentSystemCodePage();
	}

	if (srcCodePage == kGZCodePageUTF8)
	{
utf8:
		if (&src != &dest)
		{
			dest.FromChar(src.ToChar(), src.Strlen());
		}

		result = true;
	}
	else if (srcCodePage != kGZCodePageUTF16)
	{
		char* heapBuffer;
		uint32_t srcLength = src.Strlen();
		uint32_t bufferLength = srcLength * 6;
		char stackBuffer[512];

		if (bufferLength < sizeof(stackBuffer))
		{
			heapBuffer = NULL;
		}
		else
		{
			heapBuffer = new char[bufferLength];
		}

		char* buffer = heapBuffer;
		if (buffer == NULL)
		{
			buffer = stackBuffer;
		}

		buffer[0] = '\0';
		result = ConvertStringEncoding(src.Data(), srcLength, srcCodePage, buffer, bufferLength, kGZCodePageUTF8) == srcLength;

		dest.FromChar(buffer, bufferLength);
		delete[] heapBuffer;
	}

	return result;
}

bool ConvertStringEncoding(cIGZString const& src, cIGZString& dest, GZCodePage destCodePage)
{
	bool result = false;
	if (destCodePage == kGZCodePageNative)
	{
		goto utf8;
	}

	if (destCodePage == kGZCodePageSystem)
	{
		destCodePage = GetCurrentSystemCodePage();
	}

	if (destCodePage == kGZCodePageUTF8)
	{
utf8:
		if (&src != &dest)
		{
			dest.FromChar(src.ToChar(), src.Strlen());
		}

		result = true;
	}
	else
	{
		char* heapBuffer;
		uint32_t srcLength = src.Strlen();
		uint32_t bufferLength = srcLength * 6;
		char stackBuffer[512];

		if (bufferLength < sizeof(stackBuffer))
		{
			heapBuffer = NULL;
		}
		else
		{
			heapBuffer = new char[bufferLength];
		}

		char* buffer = heapBuffer;
		if (buffer == NULL)
		{
			buffer = stackBuffer;
		}

		buffer[0] = '\0';
		result = ConvertStringEncoding(src.Data(), srcLength, kGZCodePageUTF8, buffer, bufferLength, destCodePage) == srcLength;

		dest.FromChar(buffer, bufferLength);
		delete[] heapBuffer;
	}

	return result;
}

int32_t GetCharLength(void const* src, uint32_t srcLength, GZCodePage codePage)
{
	if (srcLength != 0)
	{
		if (codePage == kGZCodePageNative)
		{
			goto utf8;
		}

		if (codePage == kGZCodePageSystem)
		{
			codePage = GetCurrentSystemCodePage();
		}

		if (codePage == kGZCodePageANSI || codePage == kGZCodePageUTF16)
		{
			return 1;
		}

		if (codePage == kGZCodePageUTF8)
		{
utf8:
			unsigned char const* bytes = static_cast<unsigned char const*>(src);
			if (bytes[0] >= 0x80 && bytes[0] >= 0xc0 && bytes[0] <= 0xfd
				&& bytes[1] >= 0x80 && bytes[1] <= 0xbf)
			{
				int32_t length;
				if (bytes[0] < 0xe0)
				{
					length = 2;
				}
				else if (bytes[0] < 0xf0)
				{
					length = 3;
				}
				else if (bytes[0] < 0xf8)
				{
					length = 4;
				}
				else
				{
					return 5 + (bytes[0] >= 0xfc);
				}

				return length;
			}
		}
		else
		{
			return IsLeadByte(*static_cast<char const*>(src), codePage) ? 2 : 1;
		}

		return 1;
	}

	return 0;
}

GZCodePage GetCurrentFrameworkCodePage()
{
	return kGZCodePageUTF8;
}

GZCodePage GetCurrentSystemCodePage()
{
	static GZCodePage sSystemCodePage = kGZCodePageInvalid;

	if (sSystemCodePage == kGZCodePageInvalid)
	{
		sSystemCodePage = kGZCodePageANSI;
		char* locale = setlocale(LC_ALL, 0);

		if (locale != NULL)
		{
			if (strcmp(locale, "C") == 0)
			{
				locale = setlocale(LC_ALL, "");
			}

			if (locale != NULL)
			{
				char* separator = strstr(locale, ".");
				if (separator != NULL)
				{
					sSystemCodePage = (GZCodePage)atol(separator + 1);
				}
			}
		}
	}

	return sSystemCodePage;
}

uint32_t GetGlyphCountFromString(void const* src, uint32_t srcLength, GZCodePage codePage)
{
	uint32_t count = 0;
	if (srcLength != 0)
	{
		if (codePage == kGZCodePageNative)
		{
			goto utf8;
		}

		if (codePage == kGZCodePageSystem)
		{
			codePage = GetCurrentSystemCodePage();
		}

		if (codePage == kGZCodePageANSI || codePage == kGZCodePageUTF16)
		{
			count = srcLength;
		}
		else if (codePage == kGZCodePageUTF8)
		{
utf8:
			unsigned char const* p = static_cast<unsigned char const*>(src);
			unsigned char const* end = p + srcLength;

			while (p < end)
			{
				unsigned char ch = *p;
				if (ch < 0x80 || ch > 0xBF)
				{
					++count;
				}

				++p;
			}
		}
		else
		{
			unsigned char const* p = static_cast<unsigned char const*>(src);
			unsigned char const* end = p + srcLength;

			while (p < end)
			{
				if (IsLeadByte((char)*p, codePage))
				{
					++p;
				}

				++p;
				++count;
			}
		}
	}

	return count;
}

bool IsLeadByte(char ch, GZCodePage codePage)
{
	switch (codePage)
	{
	case kGZCodePageOEMJapanese:
		return ((unsigned char)ch >= 0x81 && (unsigned char)ch <= 0x9f)
			|| ((unsigned char)ch >= 0xe0 && (unsigned char)ch <= 0xfc);

	case kGZCodePageOEMSimplifiedChinese:
		return ((unsigned char)ch >= 0xa1 && (unsigned char)ch <= 0xfc);

	case kGZCodePageOEMKorean:
		return ((unsigned char)ch >= 0xb0 && (unsigned char)ch <= 0xc8);

	case kGZCodePageOEMTraditionalChinese:
		return ((unsigned char)ch >= 0x81 && (unsigned char)ch <= 0xfe);
	}

	return false;
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
		uint32_t charLength = (uint32_t)GetCharLength(chars + position, size - position, kGZCodePageNative);
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
		ConvertStringEncoding(src, &temp[0], length, kGZCodePageUTF16);
		result.assign(temp.begin(), temp.begin() + length);
	}
	else
	{
		wchar_t buffer[256];
		ConvertStringEncoding(src, buffer, length, kGZCodePageUTF16);
		result.assign(buffer, buffer + length);
	}

	return result;
}