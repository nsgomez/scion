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

#include "cGZPersistCacheStrategyBasic.h"
#include "cGZPersistResourceKeyFilter.h"
#include "cIGZPersistResourceKeyList.h"

cGZPersistCacheStrategyBasic::cGZPersistCacheStrategyBasic(void) :
	mainLock(),
	cacheMap(),
	gcLock(),
	ticksSinceLastGC(0),
	isGCActive(false),
	keyFilter(NULL)
{
}

void cGZPersistCacheStrategyBasic::Init(void)
{
}

void cGZPersistCacheStrategyBasic::Shutdown(void)
{
	cRZCriticalSectionHolder lock(mainLock);
	FlushCache();

	for (ResourceCacheMap::iterator it = cacheMap.begin(); it != cacheMap.end(); it++)
	{
		if (it->second != NULL)
		{
			it->second->Release();
		}
	}

	cacheMap.clear();
}

void cGZPersistCacheStrategyBasic::SetKeyFilter(cIGZPersistResourceKeyFilter* filter)
{
	keyFilter = filter;
}

cIGZPersistResourceKeyFilter* cGZPersistCacheStrategyBasic::GetKeyFilter(void)
{
	return keyFilter;
}

bool cGZPersistCacheStrategyBasic::GetResource(cGZPersistResourceKey const& key, cIGZPersistResource** outPtr)
{
	cRZCriticalSectionHolder lock(mainLock);

	ResourceCacheMap::iterator it = cacheMap.find(key);
	if (it != cacheMap.end())
	{
		*outPtr = it->second;
		it->second->AddRef();
	}

	return it != cacheMap.end();
}

bool cGZPersistCacheStrategyBasic::CacheResource(cGZPersistResourceKey const& key, cIGZPersistResource& resource)
{
	cRZCriticalSectionHolder lock(mainLock);

	ResourceCacheMap::iterator it = cacheMap.find(key);
	if (it == cacheMap.end())
	{
		resource.AddRef();
		resource.SetKey(key);
		cacheMap.insert(ResourceCacheMapEntry(key, &resource));
	}

	return it == cacheMap.end();
}

bool cGZPersistCacheStrategyBasic::UncacheResource(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(mainLock);

	ResourceCacheMap::iterator it = cacheMap.find(key);
	if (it != cacheMap.end())
	{
		it->second->Release();
		cacheMap.erase(it);
	}

	return it != cacheMap.end();
}

bool cGZPersistCacheStrategyBasic::HasCachedResource(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(mainLock);
	return cacheMap.find(key) != cacheMap.end();
}

bool cGZPersistCacheStrategyBasic::CanCacheResource(cGZPersistResourceKey const& key)
{
	cIGZPersistResourceKeyFilter* filter = keyFilter;
	if (filter != NULL)
	{
		return filter->IsKeyIncluded(key);
	}

	return true;
}

uint32_t cGZPersistCacheStrategyBasic::GetCachedResourceList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter)
{
	cRZCriticalSectionHolder lock(mainLock);
	uint32_t count = 0;

	for (ResourceCacheMap::iterator it = cacheMap.begin(); it != cacheMap.end(); it++)
	{
		cGZPersistResourceKey key = it->second->GetKey();
		bool include = false;

		if (this->keyFilter == NULL)
		{
			if (filter == NULL)
			{
				include = true;
			}
			else
			{
				include = filter->IsKeyIncluded(key);
			}
		}
		else
		{
			include = this->keyFilter->IsKeyIncluded(key);
		}

		if (include)
		{
			list->Insert(key);
		}
	}

	return count;
}

uint32_t cGZPersistCacheStrategyBasic::GetCachedResourceListForType(cIGZPersistResourceKeyList* list, uint32_t type)
{
	cGZPersistResourceKeyFilter* filter = new cGZPersistResourceKeyFilter();
	filter->AddRef();
	uint32_t result = GetCachedResourceList(list, filter);
	filter->Release();

	return result;
}

bool cGZPersistCacheStrategyBasic::OnIdle(uint32_t elapsed)
{
	if (isGCActive)
	{
		++ticksSinceLastGC;
		if (ticksSinceLastGC >= 64)
		{
			ticksSinceLastGC = 0;
			GarbageCollect();
		}
	}

	return true;
}

bool cGZPersistCacheStrategyBasic::IsGarbageCollectionActive(void)
{
	cRZCriticalSectionHolder lock(gcLock);
	return isGCActive;
}

void cGZPersistCacheStrategyBasic::SetGarbageCollectionActive(bool active)
{
	cRZCriticalSectionHolder lock(gcLock);
	isGCActive = active;
}

bool cGZPersistCacheStrategyBasic::ForceGarbageCollection(void)
{
	uint32_t nonEmptySweeps = 0;
	while (nonEmptySweeps < 8)
	{
		if (!GarbageCollect())
		{
			break;
		}

		nonEmptySweeps++;
	}

	return nonEmptySweeps != 0;
}

void cGZPersistCacheStrategyBasic::FlushCache(void)
{
	for (ResourceCacheMap::iterator it = cacheMap.begin(); it != cacheMap.end(); it++)
	{
		if (it->second != NULL)
		{
			it->second->Release();
		}
	}

	cacheMap.clear();
}

bool cGZPersistCacheStrategyBasic::GarbageCollect(void)
{
	cRZCriticalSectionHolder lock(mainLock);
	int itemsSwept = 0;

	ResourceCacheMap::iterator it = cacheMap.begin();
	while (it != cacheMap.end())
	{
		while (true)
		{
			if (it->second != NULL)
			{
				break;
			}

			ResourceCacheMap::iterator oldIt = it;
			it++;

			cacheMap.erase(oldIt);
			if (it == cacheMap.end())
			{
				return itemsSwept != 0;
			}
		}

		if (it->second->GetRefCount() > 1)
		{
			it++;
			continue;
		}

		it->second->Release();

		ResourceCacheMap::iterator oldIt = it;
		itemsSwept++;

		it->second = NULL;
		it++;

		cacheMap.erase(oldIt);
	}

	return itemsSwept != 0;
}