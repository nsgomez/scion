/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

struct cGZPersistResourceKey;
class cIGZPersistResource;
class cIGZPersistResourceKeyFilter;
class cIGZPersistResourceKeyList;

class cIGZPersistCacheStrategy : public cIGZUnknown
{
public:
	virtual void Init(void) = 0;
	virtual void Shutdown(void) = 0;

	virtual void SetKeyFilter(cIGZPersistResourceKeyFilter* filter) = 0;
	virtual cIGZPersistResourceKeyFilter* GetKeyFilter(void) = 0;

	virtual bool GetResource(cGZPersistResourceKey const& key, cIGZPersistResource** outPtr) = 0;
	virtual bool CacheResource(cGZPersistResourceKey const& key, cIGZPersistResource& resource) = 0;
	virtual bool UncacheResource(cGZPersistResourceKey const& key) = 0;
	virtual bool HasCachedResource(cGZPersistResourceKey const& key) = 0;
	virtual bool CanCacheResource(cGZPersistResourceKey const& key) = 0;

	virtual uint32_t GetCachedResourceList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter) = 0;
	virtual uint32_t GetCachedResourceListForType(cIGZPersistResourceKeyList* list, uint32_t type) = 0;

	virtual bool OnIdle(uint32_t elapsed) = 0;

	virtual bool IsGarbageCollectionActive(void) = 0;
	virtual void SetGarbageCollectionActive(bool active) = 0;
	virtual bool ForceGarbageCollection(void) = 0;

	virtual void FlushCache(void) = 0;
};