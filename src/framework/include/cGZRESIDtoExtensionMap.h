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
#include <map>
#include <string>
#include "cIGZPersistRESIDtoExtensionMap.h"
#include "cRZString.h"

class cGZRESIDtoExtensionMap : public cIGZPersistRESIDtoExtensionMap
{
public:
	cGZRESIDtoExtensionMap();
	virtual ~cGZRESIDtoExtensionMap();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool AddRESIDtoExtensionAssociation(uint32_t resId, cIGZString const& extension);
	virtual bool ExtensionToRESID(cIGZString const& extension, cGZPersistResourceKey& key);
	virtual bool RESIDToExtension(uint32_t const& resId, cIGZString& extension);

	virtual void EraseAll();
	virtual void Copy(cIGZPersistRESIDtoExtensionMap* other);

	uint32_t Size() const;

protected:
	uint32_t mRefCount;
	std::map<uint32_t, cRZString> mExtensionMap;
};