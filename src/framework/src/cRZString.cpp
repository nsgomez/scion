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

#include <cstdarg>
#include <vector>
#include "cStackString.h"
#include "cRZString.h"
#include "RZStringUtil.h"

cRZString::~cRZString()
{
}

bool cRZString::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZString:
		*outPtr = static_cast<cIGZString*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZString::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cRZString::Release(void)
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

void cRZString::FromChar(char const* otherStr)
{
	if (otherStr != NULL)
	{
		assign(otherStr, otherStr + strlen(otherStr));
	}
	else
	{
		clear();
	}
}

void cRZString::FromChar(char const* otherStr, uint32_t length)
{
	if (otherStr != NULL)
	{
		assign(otherStr, length);
	}
	else
	{
		erase();
	}
}

char const* cRZString::ToChar() const
{
	return c_str();
}

char const* cRZString::Data() const
{
	return data();
}

uint32_t cRZString::Strlen() const
{
	return length();
}

bool cRZString::IsEqual(cIGZString const* other, bool caseSensitive) const
{
	if (length() != other->Strlen())
	{
		return false;
	}

	if (caseSensitive)
	{
		return strncmp(data(), other->Data(), length()) == 0;
	}

	cRZString first(*this);
	cRZString second(other->Data(), length());

	first.MakeLower();
	second.MakeLower();

	if (first.length() != second.length())
	{
		return false;
	}

	return memcmp(first.data(), second.data(), first.length()) == 0;
}

bool cRZString::IsEqual(cIGZString const& other, bool caseSensitive) const
{
	return IsEqual(&other, caseSensitive);
}

bool cRZString::IsEqual(char const* other, uint32_t otherLen, bool caseSensitive) const
{
	if (otherLen == -1)
	{
		otherLen = strlen(other);
	}

	if (length() != otherLen)
	{
		return false;
	}

	if (caseSensitive)
	{
		return strncmp(data(), other, length()) == 0;
	}

	cRZString first(*this);
	cRZString second(other, otherLen);

	first.MakeLower();
	second.MakeLower();

	if (first.length() != second.length())
	{
		return false;
	}

	return memcmp(first.data(), second.data(), first.length()) == 0;
}

int cRZString::CompareTo(cIGZString const& other, bool caseSensitive) const
{
	uint32_t const comparisonLength = std::min(length(), other.Strlen());
	int result;

	if (caseSensitive)
	{
		result = strncmp(data(), other.Data(), comparisonLength);
	}
	else
	{
		cRZString first(*this);
		cRZString second(other.Data(), other.Strlen());

		first.MakeLower();
		second.MakeLower();

		result = memcmp(first.data(), second.data(), std::min(first.length(), second.length()));
	}

	if (result == 0)
	{
		result = length() - other.Strlen();
	}

	return result;
}

int cRZString::CompareTo(char const* other, uint32_t otherLen, bool caseSensitive) const
{
	if (otherLen == -1)
	{
		otherLen = strlen(other);
	}

	uint32_t const comparisonLength = std::min(length(), otherLen);
	int result;

	if (caseSensitive)
	{
		result = strncmp(data(), other, comparisonLength);
	}
	else
	{
		cRZString first(*this);
		cRZString second(other, otherLen);

		first.MakeLower();
		second.MakeLower();

		result = memcmp(first.data(), second.data(), std::min(first.length(), second.length()));
	}

	if (result == 0)
	{
		result = length() - otherLen;
	}

	return result;
}

int cRZString::CompareTo(cRZString const& other) const
{
	return compare(other);
}

cIGZString& cRZString::operator=(cIGZString const& other)
{
	Copy(other);
	return *this;
}

void cRZString::Copy(cIGZString const& src)
{
	FromChar(src.Data(), src.Strlen());
}

void cRZString::Resize(uint32_t newLength)
{
	resize(newLength);
}

cIGZString& cRZString::Append(char const* src, uint32_t srcLen)
{
	append(src, srcLen);
	return *this;
}

cIGZString& cRZString::Append(cIGZString const& src)
{
	append(src.ToChar(), src.Strlen());
	return *this;
}

cIGZString& cRZString::Insert(uint32_t position, char const* src, uint32_t srcLen)
{
	insert(position, src, srcLen);
	return *this;
}

cIGZString& cRZString::Insert(uint32_t position, cIGZString const& src)
{
	insert(position, src.ToChar(), src.Strlen());
	return *this;
}
	
cIGZString& cRZString::Replace(uint32_t position, char const* src, uint32_t srcLen)
{
	replace(position, srcLen, src);
	return *this;
}

cIGZString& cRZString::Replace(uint32_t position, cIGZString const& src)
{
	replace(position, src.Strlen(), src.ToChar());
	return *this;
}
	
cIGZString& cRZString::Erase(uint32_t position, uint32_t length)
{
	erase(position, length);
	return *this;
}
	
uint32_t cRZString::Find(char const* needle, uint32_t position, bool caseSensitive) const
{
	if (caseSensitive)
	{
		return find(needle, position);
	}

	cRZString first(*this);
	cRZString second(needle, strlen(needle));

	first.MakeLower();
	second.MakeLower();

	return first.find(second.c_str(), position);
}

uint32_t cRZString::Find(cIGZString const& needle, uint32_t position, bool caseSensitive) const
{
	if (caseSensitive)
	{
		return find(needle.ToChar(), position);
	}

	cRZString first(*this);
	cRZString second(needle);

	first.MakeLower();
	second.MakeLower();

	return first.find(second.c_str(), position);
}

uint32_t cRZString::Find(cRZString const& needle, uint32_t position)
{
	return find(needle, position);
}

uint32_t cRZString::RFind(char const* needle, uint32_t position, bool caseSensitive) const
{
	if (caseSensitive)
	{
		return rfind(needle, position);
	}

	cRZString first(*this);
	cRZString second(needle, strlen(needle));

	first.MakeLower();
	second.MakeLower();

	return first.rfind(second.c_str(), position);
}

uint32_t cRZString::RFind(cIGZString const& needle, uint32_t position, bool caseSensitive) const
{
	if (caseSensitive)
	{
		return rfind(needle.ToChar(), position);
	}

	cRZString first(*this);
	cRZString second(needle);

	first.MakeLower();
	second.MakeLower();

	return first.rfind(second.c_str(), position);
}

uint32_t cRZString::RFind(cRZString const& needle, uint32_t position)
{
	return rfind(needle, position);
}

cIGZString& __cdecl cRZString::Sprintf(char const* format, ...)
{
	cStackString<64> scratch;
	cStackString<1024> buffer;

	va_list ap;
	va_start(ap, format);

	char ch = *format++;
	while (ch != '\0')
	{
		if (ch == '%')
		{
			bool minusFlag = false;
			bool plusFlag = false;
			bool spaceFlag = false;
			bool zeroFlag = false;
			bool hashFlag = false;

			for (;;)
			{
				ch = *format++;
				if (ch == '-')
				{
					minusFlag = true;
					continue;
				}
				else if (ch == '+')
				{
					plusFlag = true;
					continue;
				}
				else if (ch == ' ')
				{
					spaceFlag = true;
					continue;
				}
				else if (ch == '0')
				{
					zeroFlag = true;
					continue;
				}
				else if (ch == '#')
				{
					hashFlag = true;
					continue;
				}

				break;
			}

			if (ch == '%')
			{
				buffer.push_back(ch);
				goto next;
			}

			if (minusFlag)
			{
				zeroFlag = false;
			}

			if (plusFlag)
			{
				spaceFlag = false;
			}

			int32_t width = -1;
			if (ch == '*')
			{
				width = va_arg(ap, int32_t);
				ch = *format++;
			}
			else if (isdigit(ch))
			{
				int32_t num = 0;
				do
				{
					num = num * 10 + (int32_t)ch - '0';
					ch = *format++;
				}
				while (isdigit(ch));

				width = num;
			}

			int32_t precision = -1;
			if (ch == '.')
			{
				ch = *format++;
				if (ch == '*')
				{
					precision = va_arg(ap, int32_t);
					ch = *format++;
				}
				else if (isdigit(ch))
				{
					int32_t num = 0;
					do
					{
						num = num * 10 + (int32_t)ch - '0';
						ch = *format++;
					}
					while (isdigit(ch));

					precision = num;
				}
			}

			int32_t intWidth = 1; // 0 = short int, 1 = no modifier (int), 2 = long int, 3 = long long
			if (ch == 'I')
			{
				if (format[0] == '6' && format[1] == '4')
				{
					++format;
					++format;
					ch = *format++;
					intWidth = 3;
				}
			}
			else if (ch == 'h')
			{
				ch = *format++;
				intWidth = 0;
			}
			else if (ch == 'l')
			{
				ch = *format++;
				intWidth = 2;

				if (ch == 'l')
				{
					ch = *format++;
					intWidth = 3;
				}
			}

			char fmt[6];

			char* fieldBegin = scratch.begin();
			char* fieldEnd = fieldBegin;

			bool intConversion = false;
			bool isNumeric = false;
			bool isNegative = false;

			switch (ch)
			{
			case 'd':
			case 'i':
				isNumeric = true;

			case 'o':
			case 'u':
			case 'x':
			case 'X':
				intConversion = true;
				fmt[0] = '%';

				switch (intWidth)
				{
				case 0:
					{
						int16_t value = va_arg(ap, int16_t);
						if (value < 0 && isNumeric)
						{
							isNegative = true;
							value = -value;
							ch = 'u';
						}

						fmt[1] = 'h';
						fmt[2] = ch;
						fmt[3] = '\0';
						fieldEnd = fieldBegin + sprintf(scratch.begin(), fmt, value);
					}

					break;

				case 1:
					{
						int32_t value = va_arg(ap, int32_t);
						if (value < 0 && isNumeric)
						{
							isNegative = true;
							value = -value;
							ch = 'u';
						}

						fmt[1] = ch;
						fmt[2] = '\0';
						fieldEnd = fieldBegin + sprintf(scratch.begin(), fmt, value);
					}

					break;

				case 2:
					{
						long value = va_arg(ap, long);
						if (value < 0 && isNumeric)
						{
							isNegative = true;
							value = -value;
							ch = 'u';
						}

						fmt[1] = 'l';
						fmt[2] = ch;
						fmt[3] = '\0';
						fieldEnd = fieldBegin + sprintf(scratch.begin(), fmt, value);
					}

					break;

				case 3:
					{
						int64_t value = va_arg(ap, int64_t);
						if (value < 0 && isNumeric)
						{
							isNegative = true;
							value = -value;
							ch = 'u';
						}

						fmt[1] = 'I';
						fmt[2] = '6';
						fmt[3] = '4';
						fmt[4] = ch;
						fmt[5] = '\0';
						fieldEnd = fieldBegin + sprintf(scratch.begin(), fmt, value);
					}

					break;
				}

				break;

			case 'c':
				*fieldBegin = (char)va_arg(ap, int32_t);
				fieldEnd = fieldBegin + 1;
				break;

			case 'e':
			case 'f':
			case 'g':
			case 'E':
			case 'G':
				{
					char* f = fmt;
					*f++ = '%';

					if (hashFlag)
					{
						*f++ = '#';
					}

					if (precision >= 0)
					{
						*f++ = '.';
						f += sprintf(f, "%d", precision);
					}

					double value = va_arg(ap, double);

					*f++ = ch;
					*f = '\0';
					isNumeric = true;

					if (value < 0.0)
					{
						isNegative = true;
						value = -value;
					}

					for (;;)
					{
						int32_t written = _snprintf(scratch.begin(), (uint32_t)(scratch.end() - scratch.begin()), fmt, value);
						if (written >= 0)
						{
							fieldBegin = scratch.begin();
							fieldEnd = fieldBegin + written;
							break;
						}

						scratch.resize(2u * (uint32_t)(scratch.cap(), scratch.begin()));
					}
				}

				break;

			case 'p':
				fieldEnd = fieldBegin + sprintf(scratch.begin(), "%p", va_arg(ap, void*));
				break;

			case 'n':
				*va_arg(ap, int32_t*) = (int32_t)length();
				goto next;

			case 's':
				{
					char* str = va_arg(ap, char*);
					fieldBegin = str;

					int32_t length = (int32_t)strlen(str);
					if (precision >= 0)
					{
						int32_t used = 0;
						int32_t glyphs = 0;

						while (used < length)
						{
							if (glyphs >= precision)
							{
								break;
							}

							int32_t charLen = GetCharLength(fieldBegin + used, (uint32_t)(length - used), kGZCodePageNative);
							used += charLen;
							++glyphs;
						}

						length = used;
					}

					fieldEnd = fieldBegin + length;
				}

				break;

			case 'C':
			case 'S':
				// Wide-character conversions are not supported.

			default:
				--format;
				break;
			}

			// TODO
			int32_t const byteLength = (int32_t)(fieldEnd - fieldBegin);
			uint32_t glyphCount = GetGlyphCountFromString(fieldBegin, (uint32_t)byteLength, kGZCodePageNative);

			int32_t zeroPadding = 0;
			int32_t leftPadding = 0;

			char sign = '\0';
			char const* prefix = NULL;
			uint32_t prefixLength;

			if (intConversion)
			{
				if (precision > (int32_t)glyphCount)
				{
					zeroPadding = precision - (int32_t)glyphCount;
				}

				if (hashFlag)
				{
					if (ch == 'X')
					{
						prefix = "0X";
						prefixLength = 2;
						glyphCount += 2;
					}
					else if (ch == 'o')
					{
						prefix = "0";
						prefixLength = 1;
						glyphCount += 1;
					}
					else if (ch == 'x')
					{
						prefix = "0x";
						prefixLength = 2;
						glyphCount += 2;
					}
				}
			}

			if (isNumeric)
			{
				if (isNegative)
				{
					sign = '-';
				}
				else
				{
					if (spaceFlag)
					{
						sign = ' ';
					}

					if (plusFlag)
					{
						sign = '+';
					}
				}

				if (sign != '\0')
				{
					++glyphCount;
					if (zeroFlag)
					{
						buffer.push_back(sign);
					}
				}
			}

			if (width > 0)
			{
				leftPadding = width - zeroPadding - (int32_t)glyphCount;
				if (leftPadding > 0 && !minusFlag)
				{
					buffer.append((uint32_t)leftPadding, zeroFlag ? '0' : ' ');
				}
			}

			if (sign != '\0' && !zeroFlag)
			{
				buffer.push_back(sign);
			}

			if (prefix != NULL)
			{
				buffer.append(prefix, prefixLength);
			}

			if (zeroPadding > 0)
			{
				buffer.append((uint32_t)zeroPadding, '0');
			}

			buffer.append(fieldBegin, (uint32_t)byteLength);

			if (leftPadding > 0 && minusFlag)
			{
				buffer.append((uint32_t)leftPadding, ' ');
			}
		}
		else
		{
			char const* start = format - 1;
			while (*format != 0)
			{
				if (*format == '%')
				{
					break;
				}

				++format;
			}

			buffer.append(start, (uint32_t)(format - start));
		}

next:
		ch = *format++;
	}

	assign(buffer.begin(), buffer.end());
	return *this;
}

bool cRZString::BeginsWith(char const* needle, uint32_t needleLen) const
{
	if (needleLen <= length())
	{
		for (uint32_t i = 0; i < needleLen; i++)
		{
			if ((*this)[i] != needle[i])
			{
				return false;
			}
		}
	}

	return false;
}

bool cRZString::EndsWith(char const* needle, uint32_t needleLen) const
{
	uint32_t thisLen = length();
	if (needleLen <= thisLen)
	{
		for (uint32_t i = 1; i <= needleLen; i++)
		{
			if (c_str()[thisLen - i] != needle[needleLen - i])
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

cRZString cRZString::Left(uint32_t count) const
{
	if (count > length())
	{
		return *this;
	}
	else
	{
		return Mid(0, count);
	}
}

cRZString cRZString::Mid(uint32_t position, uint32_t count) const
{
	if (position >= length())
	{
		return cRZString();
	}
	else
	{
		return cRZString(*this, position, std::min(length() - position, count));
	}
}

cRZString cRZString::Right(uint32_t count) const
{
	if (count > length())
	{
		return *this;
	}
	else
	{
		return Mid(length() - count, count);
	}
}
	
void cRZString::Trim()
{
	LTrim();
	RTrim();
}

void cRZString::LTrim()
{
	erase(0, find_first_not_of(" \t", 0));
}

void cRZString::RTrim()
{
	erase(find_last_not_of(" \t", -1) + 1, -1);
}

void cRZString::MakeLower()
{
	iterator itEnd = end();
	for (iterator it = begin(); it != itEnd; ++it)
	{
		char c = *it;
		if ((unsigned char)c >= 0x7f)
		{
			size_t length = size();

			std::vector<wchar_t> heapBuffer;
			wchar_t stackBuffer[256];
			wchar_t* wideStr = stackBuffer;

			if (length > 256)
			{
				heapBuffer.resize(length);
				wideStr = &heapBuffer[0];
			}

			ConvertStringEncoding(*this, wideStr, length, kGZCodePageUTF16);
			for (size_t i = 0; i < length; i++)
			{
				wideStr[i] = towlower(wideStr[i]);
			}

			ConvertStringEncoding(wideStr, length, kGZCodePageUTF16, *this);
			break;
		}
		
		if (isupper(c))
		{
			c = (char)tolower(c);
		}

		*it = c;
	}
}

void cRZString::MakeUpper()
{
	iterator itEnd = end();
	for (iterator it = begin(); it != itEnd; ++it)
	{
		char c = *it;
		if ((unsigned char)c >= 0x7f)
		{
			size_t length = size();

			std::vector<wchar_t> heapBuffer;
			wchar_t stackBuffer[256];
			wchar_t* wideStr = stackBuffer;

			if (length > 256)
			{
				heapBuffer.resize(length);
				wideStr = &heapBuffer[0];
			}

			ConvertStringEncoding(*this, wideStr, length, kGZCodePageUTF16);
			for (size_t i = 0; i < length; i++)
			{
				wideStr[i] = towupper(wideStr[i]);
			}

			ConvertStringEncoding(wideStr, length, kGZCodePageUTF16, *this);
			break;
		}
		
		if (islower(c))
		{
			c = (char)toupper(c);
		}

		*it = c;
	}
}

cRZString cRZString::SplitToken(char const* delimiter)
{
	if (delimiter == NULL)
	{
		return *this;
	}

	size_t delimiterLen = strlen(delimiter);
	if (delimiterLen == 0)
	{
		return *this;
	}

	uint32_t pos = find(delimiter, 0, delimiterLen);
	if (pos == std::string::npos)
	{
		return cRZString();
	}
	else
	{
		cRZString token(*this, 0, pos);
		erase(0, pos + delimiterLen);
		return token;
	}
}

bool cRZString::SplitTokenDelimited(char delimiter)
{
	uint32_t pos = find(delimiter);
	if (pos == std::string::npos)
	{
		erase();
	}
	else
	{
		erase(0, pos + 1);
	}

	return pos != std::string::npos;
}

bool cRZString::SplitTokenDelimited(char delimiter, cRZString& token)
{
	uint32_t pos = find(delimiter);
	if (pos == std::string::npos)
	{
		token.erase();
		swap(token);
		return false;
	}
	
	token.assign(*this, 0, pos);
	erase(0, pos + 1);
	return true;
}

bool cRZString::SplitTokenSeparated(char delimiter, cRZString& token)
{
	while (true)
	{
		uint32_t delimiterPos = find(delimiter);
		if (delimiterPos == std::string::npos)
		{
			if (empty())
			{
				return false;
			}
			
			token.erase();
			token.swap(*this);
			return true;
		}

		uint32_t nextTokenPos = find_first_not_of(delimiter, delimiterPos);
		if (delimiterPos > 0)
		{
			token.assign(*this, 0, delimiterPos);
			erase(0, nextTokenPos);
			return true;
		}

		erase(0, nextTokenPos);
	}
}

cRZString& cRZString::Strcat(char const* str)
{
	append(str);
	return *this;
}

cRZString& cRZString::Strncpy(char const* str, uint32_t count)
{
	uint32_t len = strlen(str);
	char const* end = (count > len) ? (str + len) : (str + count);
	assign(str, end);
	return *this;
}

bool cRZString::operator< (const cIGZString& other) const
{
	return CompareTo(other) < 0;
}

bool cRZString::operator> (const cIGZString& other) const
{
	return CompareTo(other) > 0;
}

bool cRZString::operator== (const cIGZString& other) const
{
	return CompareTo(other) == 0;
}
	
bool cRZString::operator< (char const* other) const
{
	return CompareTo(other) < 0;
}

bool cRZString::operator> (char const* other) const
{
	return CompareTo(other) > 0;
}

bool cRZString::operator== (char const* other) const
{
	return CompareTo(other) == 0;
}