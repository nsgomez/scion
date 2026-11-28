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

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZRegistry = 0xA237613C;
static const GZGUID RZSRVID_cGZRegistry = 0x25076B9E; // TODO: move

class cIGZString;

class cIGZRegistry : public cIGZUnknown
{
public:
	typedef void (*GZRegistryCallbackFn)(cIGZString const& name, cIGZString const& value, void* context);

public:
	virtual bool Register(cIGZString const&) = 0;
	virtual bool Unregister(cIGZString const&) = 0;

	virtual bool MakeRegistryName(char const*, cIGZString const&, cIGZString const&, cIGZString&) = 0;
	virtual bool Lookup(char const*, cIGZString const&, cIGZString&) = 0;
	virtual bool EnumCategory(char const*, GZRegistryCallbackFn callback, void* context);
};