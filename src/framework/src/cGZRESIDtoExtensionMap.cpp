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

#include "cGZRESIDtoExtensionMap.h"

cGZRESIDtoExtensionMap::cGZRESIDtoExtensionMap()
{
}

cGZRESIDtoExtensionMap::~cGZRESIDtoExtensionMap()
{
}

bool cGZRESIDtoExtensionMap::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZPersistRESIDtoExtensionMap:
		*outPtr = static_cast<cIGZPersistRESIDtoExtensionMap*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	default:
		return false;
	}
}

uint32_t cGZRESIDtoExtensionMap::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZRESIDtoExtensionMap::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cGZRESIDtoExtensionMap::AddRESIDtoExtensionAssociation(uint32_t resId, cIGZString const& extension)
{
	mExtensionMap.insert(std::pair<uint32_t, cRZString>(resId, cRZString(extension)));
	return true;
}

bool cGZRESIDtoExtensionMap::ExtensionToRESID(cIGZString const& extension, cGZPersistResourceKey& key)
{
	std::map<uint32_t, cRZString>::iterator end = mExtensionMap.end();
	for (std::map<uint32_t, cRZString>::iterator it = mExtensionMap.begin(); it != end; it++)
	{
		// TODO
	}

	return false;
}

bool cGZRESIDtoExtensionMap::RESIDToExtension(uint32_t const& resId, cIGZString& extension)
{
	// TODO
	return false;
}

void cGZRESIDtoExtensionMap::EraseAll()
{
	mExtensionMap.clear();
}

void cGZRESIDtoExtensionMap::Copy(cIGZPersistRESIDtoExtensionMap* other)
{
	EraseAll();

	// TODO: is this really what the game is doing??
	cGZRESIDtoExtensionMap* otherAsMap = reinterpret_cast<cGZRESIDtoExtensionMap*>(other);

	std::map<uint32_t, cRZString>::iterator end = otherAsMap->mExtensionMap.end();
	for (std::map<uint32_t, cRZString>::iterator it = otherAsMap->mExtensionMap.begin(); it != end; it++)
	{
		AddRESIDtoExtensionAssociation(it->first, it->second);
	}
}

uint32_t cGZRESIDtoExtensionMap::Size() const
{
	return mExtensionMap.size();
}