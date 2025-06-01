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

#include "cGZPersistResourceKeyList.h"

cGZPersistResourceKeyList::cGZPersistResourceKeyList(void) :
	refCount(0),
	keys()
{
}

bool cGZPersistResourceKeyList::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistResourceKeyList)
	{
		*outPtr = static_cast<cIGZPersistResourceKeyList*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistResourceKeyList::AddRef()
{
	return ++refCount;
}

uint32_t cGZPersistResourceKeyList::Release()
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

bool cGZPersistResourceKeyList::Insert(cGZPersistResourceKey const& key)
{
	keys.push_back(key);
	return true;
}

bool cGZPersistResourceKeyList::Insert(cIGZPersistResourceKeyList const& list)
{
	list.EnumKeys(InsertKeyCallback, this);
	return true;
}

bool cGZPersistResourceKeyList::Erase(cGZPersistResourceKey const& key)
{
	ResourceKeyDeque::iterator it = std::find(keys.begin(), keys.end(), key);
	if (it != keys.end())
	{
		keys.erase(it);
	}

	return true;
}

bool cGZPersistResourceKeyList::EraseAll(void)
{
	keys.clear();
	return true;
}

void cGZPersistResourceKeyList::EnumKeys(tKeyListIterator fn, void* context) const
{
	for (ResourceKeyDeque::const_iterator it = keys.begin(); it != keys.end(); it++)
	{
		fn((*it), context);
	}
}

bool cGZPersistResourceKeyList::IsPresent(cGZPersistResourceKey const& key)
{
	ResourceKeyDeque::iterator it = std::find(keys.begin(), keys.end(), key);
	return it != keys.end();
}

uint32_t cGZPersistResourceKeyList::Size(void) const
{
	return keys.size();
}

cGZPersistResourceKey cGZPersistResourceKeyList::GetKey(uint32_t index) const
{
	ResourceKeyDeque::const_iterator it = keys.begin();
	it += index;
	return *it;
}

void cGZPersistResourceKeyList::InsertKeyCallback(cGZPersistResourceKey const& key, void* context)
{
	cGZPersistResourceKeyList* self = reinterpret_cast<cGZPersistResourceKeyList*>(context);
	self->keys.push_back(key);
}