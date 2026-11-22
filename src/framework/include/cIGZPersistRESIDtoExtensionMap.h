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

static const GZIID GZIID_cIGZPersistRESIDtoExtensionMap = 0xA56C8CFD;

struct cGZPersistResourceKey;
class cIGZString;

class cIGZPersistRESIDtoExtensionMap : public cIGZUnknown
{
public:
	virtual bool AddRESIDtoExtensionAssociation(uint32_t resId, cIGZString const& extension) = 0;
	virtual bool ExtensionToRESID(cIGZString const& extension, cGZPersistResourceKey& key) = 0;
	virtual bool RESIDToExtension(uint32_t const& resId, cIGZString& extension) = 0;

	virtual void EraseAll() = 0;
	virtual void Copy(cIGZPersistRESIDtoExtensionMap* other) = 0;
};