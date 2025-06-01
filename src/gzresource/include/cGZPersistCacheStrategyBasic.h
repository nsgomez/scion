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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <hash_map>
#include "cGZPersistResourceKey.h"
#include "cIGZPersistCacheStrategy.h"
#include "cIGZPersistResource.h"
#include "cIGZPersistResourceKeyFilter.h"
#include "cRZAutoRefCount.h"
#include "cRZCriticalSection.h"

struct ResourceKeyHash
{
	size_t operator()(cGZPersistResourceKey const key) const { return key.type; }
};

class cGZPersistCacheStrategyBasic : public cIGZPersistCacheStrategy
{
public:
	cGZPersistCacheStrategyBasic(void);
	virtual ~cGZPersistCacheStrategyBasic(void) { }

public:
	virtual void Init(void);
	virtual void Shutdown(void);

	virtual void SetKeyFilter(cIGZPersistResourceKeyFilter* filter);
	virtual cIGZPersistResourceKeyFilter* GetKeyFilter(void);

	virtual bool GetResource(cGZPersistResourceKey const& key, cIGZPersistResource** outPtr);
	virtual bool CacheResource(cGZPersistResourceKey const& key, cIGZPersistResource& resource);
	virtual bool UncacheResource(cGZPersistResourceKey const& key);
	virtual bool HasCachedResource(cGZPersistResourceKey const& key);
	virtual bool CanCacheResource(cGZPersistResourceKey const& key);

	virtual uint32_t GetCachedResourceList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter);
	virtual uint32_t GetCachedResourceListForType(cIGZPersistResourceKeyList* list, uint32_t type);

	virtual bool OnIdle(uint32_t elapsed);

	virtual bool IsGarbageCollectionActive(void);
	virtual void SetGarbageCollectionActive(bool active);
	virtual bool ForceGarbageCollection(void);

	virtual void FlushCache(void);

protected:
	virtual bool GarbageCollect(void);

protected:
	typedef std::hash_map<cGZPersistResourceKey, cIGZPersistResource*, ResourceKeyHash> ResourceCacheMap;
	typedef std::pair<cGZPersistResourceKey, cIGZPersistResource*> ResourceCacheMapEntry;

	cRZCriticalSection mainLock;
	ResourceCacheMap cacheMap;
	cRZCriticalSection gcLock;
	uint32_t ticksSinceLastGC;
	bool isGCActive;
	cRZAutoRefCount<cIGZPersistResourceKeyFilter> keyFilter;
};