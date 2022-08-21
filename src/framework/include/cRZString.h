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

#pragma once
#include <string>
#include "cIGZString.h"

class cRZString : public std::string, public cIGZString
{
public:
	cRZString();
	cRZString(const char* src);
	cRZString(const char* src, uint32_t len);
	cRZString(cIGZString const& src);
	cRZString(cIGZString const* src);
	virtual ~cRZString();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual void FromChar(char const* otherStr);
	virtual void FromChar(char const* otherStr, uint32_t length);

	virtual char const* ToChar() const;
	virtual char const* Data() const;
	virtual uint32_t Strlen() const;

	virtual bool IsEqual(cIGZString const* other, bool caseSensitive = true) const;
	virtual bool IsEqual(cIGZString const& other, bool caseSensitive = true) const;
	virtual bool IsEqual(char const* other, uint32_t otherLen, bool caseSensitive = true) const;

	virtual int CompareTo(cIGZString const& other, bool caseSensitive = true) const;
	virtual int CompareTo(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const;
	int CompareTo(cRZString const& other) const;

	virtual cIGZString& operator=(cIGZString const& other);
	virtual cRZString& operator=(cRZString const& other);
	virtual void Copy(cIGZString const& src);

	virtual void Resize(uint32_t newLength);

	virtual cIGZString& Append(char const* src, uint32_t srcLen);
	virtual cIGZString& Append(cIGZString const& src);

	virtual cIGZString& Insert(uint32_t position, char const* src, uint32_t srcLen);
	virtual cIGZString& Insert(uint32_t position, cIGZString const& src);

	virtual cIGZString& Replace(uint32_t position, char const* src, uint32_t srcLen);
	virtual cIGZString& Replace(uint32_t position, cIGZString const& src);

	virtual cIGZString& Erase(uint32_t position = 0, uint32_t length = -1);

	virtual uint32_t Find(char const* needle, uint32_t position = 0, bool caseSensitive = true) const;
	virtual uint32_t Find(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const;
	uint32_t Find(cRZString const& needle, uint32_t position = 0);

	virtual uint32_t RFind(char const* needle, uint32_t position = 0, bool caseSensitive = true) const;
	virtual uint32_t RFind(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const;
	uint32_t RFind(cRZString const& needle, uint32_t position = 0);

	virtual void Sprintf(char const* format, ...);

public:
	bool BeginsWith(char const* needle, uint32_t needleLen) const;
	bool EndsWith(char const* needle, uint32_t needleLen) const;

	void Left(uint32_t) const;
	void Mid(uint32_t, uint32_t) const;
	void Right(uint32_t) const;
	
	void Trim();
	void LTrim();
	void RTrim();

	void MakeLower();
	void MakeUpper();

	void SplitToken(char const* token);
	bool SplitTokenDelimited(char);
	bool SplitTokenDelimited(char, cRZString&);
	bool SplitTokenSeparated(cRZString&);

	void Strcat(char const* str);
	void Strncpy(char const* str, uint32_t count);

public:
	bool operator<  (const cIGZString& other) const;
	bool operator>  (const cIGZString& other) const;
	bool operator== (const cIGZString& other) const;

	bool operator<  (char const* other) const;
	bool operator>  (char const* other) const;
	bool operator== (char const* other) const;

protected:
	uint32_t refCount;
};