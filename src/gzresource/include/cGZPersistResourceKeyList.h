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
#include <deque>
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceKeyList.h"

static const GZGUID GZCLSID_cGZPersistResourceKeyList = 0x656B8F2D;

class cGZPersistResourceKeyList : public cIGZPersistResourceKeyList
{
public:
	cGZPersistResourceKeyList(void);
	virtual ~cGZPersistResourceKeyList(void) { }

	friend class cGZDBSegmentPackedFile;

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Insert(cGZPersistResourceKey const& key);
	virtual bool Insert(cIGZPersistResourceKeyList const& list);

	virtual bool Erase(cGZPersistResourceKey const& key);
	virtual bool EraseAll(void);

	virtual void EnumKeys(tKeyListIterator fn, void* context) const;

	virtual bool IsPresent(cGZPersistResourceKey const& key);
	virtual uint32_t Size(void) const;
	virtual cGZPersistResourceKey GetKey(uint32_t index) const;

protected:
	static void InsertKeyCallback(cGZPersistResourceKey const& key, void* context);

	typedef std::deque<cGZPersistResourceKey> ResourceKeyDeque;

	uint32_t refCount;
	ResourceKeyDeque keys;
};