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

#include <algorithm>
#include <ctype.h>
#include <vector>
#include "cRZString.h"
#include "RZStringUtil.h"

cRZString::cRZString() :
	std::string(),
	mRefCount(0)
{
}

cRZString::cRZString(const char* src) :
	std::string(src),
	mRefCount(0)
{
}

cRZString::cRZString(const char* src, uint32_t len) :
	std::string(src, len),
	mRefCount(0)
{
}

cRZString::cRZString(std::string const& src, uint32_t pos, uint32_t count) :
	std::string(src, pos, count),
	mRefCount(0)
{
}

cRZString::cRZString(cIGZString const& src) :
	std::string(src.Data(), src.Strlen()),
	mRefCount(0)
{
}

cRZString::cRZString(cIGZString const* src)
{
	if (src != NULL)
	{
		assign(src->Data());
	}
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
		assign(otherStr);
	}
	else
	{
		clear();
	}
}

void cRZString::FromChar(char const* otherStr, uint32_t length)
{
	if (otherStr == NULL)
	{
		erase();
	}
	else
	{
		assign(otherStr, length);
	}
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

	return memcmp(first.data(), second.data(), first.length());
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
		cRZString second(other);

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

cRZString& cRZString::operator=(cRZString const& other)
{
	assign(other);
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

void cRZString::Sprintf(char const* format, ...)
{
	// TODO
}

bool cRZString::BeginsWith(char const* needle, uint32_t needleLen) const
{
	size_t thisLen = length();
	if (needleLen > thisLen)
	{
		return false;
	}

	for (uint32_t i = 0; i < needleLen; i++)
	{
		if ((*this)[i] != needle[i])
		{
			return false;
		}
	}

	return true;
}

bool cRZString::EndsWith(char const* needle, uint32_t needleLen) const
{
	size_t thisLen = length();
	if (needleLen > thisLen)
	{
		return false;
	}
	else if (needleLen == 0)
	{
		return true;
	}

	char const* haystack = c_str() + thisLen - 1;
	needle = needle + needleLen - 1;

	for (uint32_t i = 0; i < needleLen; i++)
	{
		if (*(haystack--) != *(needle--))
		{
			return false;
		}
	}

	return true;
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
		if (c >= 0x7f)
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

			ConvertStringEncoding(*this, wideStr, length, GZ_CODEPAGE_UTF16);
			for (size_t i = 0; i < length; i++)
			{
				wideStr[i] = towlower(wideStr[i]);
			}

			ConvertStringEncoding(wideStr, length, GZ_CODEPAGE_UTF16, *this);
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
		if (c >= 0x7f)
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

			ConvertStringEncoding(*this, wideStr, length, GZ_CODEPAGE_UTF16);
			for (size_t i = 0; i < length; i++)
			{
				wideStr[i] = towupper(wideStr[i]);
			}

			ConvertStringEncoding(wideStr, length, GZ_CODEPAGE_UTF16, *this);
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
	size_t delimiterLen = strlen(delimiter);
	if (delimiter == NULL || delimiterLen == 0)
	{
		return cRZString(*this);
	}

	uint32_t pos = find(delimiter);
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

void cRZString::Strcat(char const* str)
{
	append(str);
}

void cRZString::Strncpy(char const* str, uint32_t count)
{
	uint32_t len = strlen(str);
	if (len < count)
	{
		count = len;
	}

	assign(str, count);
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