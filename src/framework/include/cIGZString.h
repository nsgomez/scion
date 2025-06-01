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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZString = 0x089B7DC8;

class cIGZString : public cIGZUnknown
{
public:
	virtual void FromChar(char const* otherStr) = 0;
	virtual void FromChar(char const* otherStr, uint32_t length) = 0;

	virtual char const* ToChar(void) const = 0;
	virtual char const* Data(void) const = 0;
	virtual uint32_t Strlen(void) const = 0;

	virtual bool IsEqual(cIGZString const*, bool caseSensitive = true) const = 0;
	virtual bool IsEqual(cIGZString const&, bool caseSensitive = true) const = 0;
	virtual bool IsEqual(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const = 0;

	virtual int CompareTo(cIGZString const& other, bool caseSensitive = true) const = 0;
	virtual int CompareTo(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const = 0;

	virtual cIGZString& operator=(cIGZString const& other) = 0;
	virtual void Copy(cIGZString const& src) = 0;

	virtual void Resize(uint32_t newLength) = 0;

	virtual cIGZString& Append(char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Append(cIGZString const& src) = 0;

	virtual cIGZString& Insert(uint32_t position, char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Insert(uint32_t position, cIGZString const& src) = 0;

	virtual cIGZString& Replace(uint32_t position, char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Replace(uint32_t position, cIGZString const& src) = 0;

	virtual cIGZString& Erase(uint32_t position = 0, uint32_t length = -1) = 0;

	virtual uint32_t Find(char const* needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t Find(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t RFind(char const* needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t RFind(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const = 0;

	virtual void Sprintf(char const* format, ...) = 0;
};