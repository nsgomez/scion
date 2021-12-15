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

#include "cRZString.h"

cRZString::cRZString()
{
}

cRZString::cRZString(const char* src) :
	str(src)
{
}

cRZString::cRZString(const char* src, uint32_t len) :
	str(src, len)
{
}

cRZString::cRZString(cIGZString const& src) :
	str(src.Data())
{
}

cRZString::cRZString(cIGZString const* src)
{
	if (src != NULL)
	{
		str.assign(src->Data());
	}
}

cRZString::~cRZString()
{
}

bool cRZString::QueryInterface(GZREFIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZString)
	{
		*outPtr = static_cast<cIGZString*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZString::AddRef(void)
{
	return ++refCount;
}

uint32_t cRZString::Release(void)
{
	return --refCount;
}

void cRZString::FromChar(char const* otherStr)
{
	if (otherStr == NULL)
	{
		str.erase();
	}
	else
	{
		str.assign(otherStr);
	}
}

void cRZString::FromChar(char const* otherStr, uint32_t length)
{
	if (otherStr == NULL)
	{
		str.erase();
	}
	else
	{
		str.assign(otherStr, length);
	}
}

char const* cRZString::ToChar() const
{
	return str.c_str();
}

char const* cRZString::Data() const
{
	return str.c_str();
}

uint32_t cRZString::Strlen() const
{
	return str.length();
}

bool cRZString::IsEqual(cIGZString const* other, bool caseSensitive) const
{
	if (str.length() != other->Strlen())
	{
		return false;
	}

	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseOther(other);

		lowercaseThis.MakeLower();
		lowercaseOther.MakeLower();

		return lowercaseThis.str == lowercaseOther.str;
	}
	else
	{
		char const* otherData = other->Data();
		return strncmp(str.c_str(), otherData, str.length()) == 0;
	}
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

	if (str.length() != otherLen)
	{
		return false;
	}

	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseOther(other, otherLen);

		lowercaseThis.MakeLower();
		lowercaseOther.MakeLower();

		return lowercaseThis.str == lowercaseOther.str;
	}
	else
	{
		return strncmp(str.c_str(), other, otherLen) == 0;
	}
}

int cRZString::CompareTo(cIGZString const& other, bool caseSensitive) const
{
	// TODO
	return 0;
}

int cRZString::CompareTo(char const* other, uint32_t otherLen, bool caseSensitive) const
{
	// TODO
	return 0;
}

int cRZString::CompareTo(cRZString const& other) const
{
	return str.compare(other.str);
}

cIGZString& cRZString::operator=(cIGZString const& other)
{
	Copy(other);
	return *this;
}

cRZString& cRZString::operator=(cRZString const& other)
{
	str.assign(other.str);
	return *this;
}

void cRZString::Copy(cIGZString const& src)
{
	FromChar(src.Data(), src.Strlen());
}

void cRZString::Resize(uint32_t newLength)
{
	str.resize(newLength);
}

cIGZString& cRZString::Append(char const* src, uint32_t srcLen)
{
	str.append(src, srcLen);
	return *this;
}

cIGZString& cRZString::Append(cIGZString const& src)
{
	str.append(src.ToChar(), src.Strlen());
	return *this;
}

cIGZString& cRZString::Insert(uint32_t position, char const* src, uint32_t srcLen)
{
	str.insert(position, src, srcLen);
	return *this;
}

cIGZString& cRZString::Insert(uint32_t position, cIGZString const& src)
{
	str.insert(position, src.ToChar(), src.Strlen());
	return *this;
}
	
cIGZString& cRZString::Replace(uint32_t position, char const* src, uint32_t srcLen)
{
	str.replace(position, srcLen, src);
	return *this;
}

cIGZString& cRZString::Replace(uint32_t position, cIGZString const& src)
{
	str.replace(position, src.Strlen(), src.ToChar());
	return *this;
}
	
cIGZString& cRZString::Erase(uint32_t position, uint32_t length)
{
	str.erase(position, length);
	return *this;
}
	
uint32_t cRZString::Find(char const* needle, uint32_t position, bool caseSensitive) const
{
	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseNeedle(needle);

		lowercaseThis.MakeLower();
		lowercaseNeedle.MakeLower();

		return lowercaseThis.str.find(lowercaseNeedle.str, position);
	}
	else
	{
		return str.find(needle, position);
	}
}

uint32_t cRZString::Find(cIGZString const& needle, uint32_t position, bool caseSensitive) const
{
	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseNeedle(needle);

		lowercaseThis.MakeLower();
		lowercaseNeedle.MakeLower();

		return lowercaseThis.str.find(lowercaseNeedle.str, position);
	}
	else
	{
		return str.find(needle.ToChar(), position);
	}
}

uint32_t cRZString::Find(cRZString const& needle, uint32_t position)
{
	return str.find(needle.str, position);
}

uint32_t cRZString::RFind(char const* needle, uint32_t position, bool caseSensitive) const
{
	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseNeedle(needle);

		lowercaseThis.MakeLower();
		lowercaseNeedle.MakeLower();

		return lowercaseThis.str.rfind(lowercaseNeedle.str, position);
	}
	else
	{
		return str.rfind(needle, position);
	}
}

uint32_t cRZString::RFind(cIGZString const& needle, uint32_t position, bool caseSensitive) const
{
	if (!caseSensitive)
	{
		cRZString lowercaseThis(this);
		cRZString lowercaseNeedle(needle);

		lowercaseThis.MakeLower();
		lowercaseNeedle.MakeLower();

		return lowercaseThis.str.rfind(lowercaseNeedle.str, position);
	}
	else
	{
		return str.rfind(needle.ToChar(), position);
	}
}

uint32_t cRZString::RFind(cRZString const& needle, uint32_t position)
{
	return str.rfind(needle.str, position);
}

void cRZString::Sprintf(char const* format, ...)
{
	// TODO
}

bool cRZString::BeginsWith(char const* needle, uint32_t needleLen) const
{
	// TODO
	return false;
}

bool cRZString::EndsWith(char const* needle, uint32_t needleLen) const
{
	// TODO
	return false;
}

void cRZString::Left(uint32_t) const
{
	// TODO
}

void cRZString::Mid(uint32_t, uint32_t) const
{
	// TODO
}

void cRZString::Right(uint32_t) const
{
	// TODO
}
	
void cRZString::Trim()
{
	uint32_t index = str.find_first_not_of(" \t", 0, 2);
	str.erase(0, index);

	index = str.find_last_not_of(" \t", -1, 2);
	str.erase(index + 1, -1);
}

void cRZString::LTrim()
{
	uint32_t index = str.find_first_not_of(" \t", 0, 2);
	str.erase(0, str.find_first_not_of(" \t", 0, 2));
}

void cRZString::RTrim()
{
	uint32_t index = str.find_last_not_of(" \t", -1, 2);
	str.erase(index + 1, -1);
}

void cRZString::MakeLower()
{
	// TODO
}

void cRZString::MakeUpper()
{
	// TODO
}

void cRZString::SplitToken(char const* token)
{
	// TODO
}

bool cRZString::SplitTokenDelimited(char)
{
	// TODO
	return false;
}

bool cRZString::SplitTokenDelimited(char, cRZString&)
{
	// TODO
	return false;
}

bool cRZString::SplitTokenSeparated(cRZString&)
{
	// TODO
	return false;
}

void cRZString::Strcat(char const* str)
{
	this->str.append(str);
}

void cRZString::Strncpy(char const* str, uint32_t count)
{
	uint32_t len = strlen(str);
	if (len < count)
	{
		count = len;
	}

	this->str.assign(str, count);
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