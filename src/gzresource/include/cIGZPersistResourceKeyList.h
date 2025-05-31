/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License.
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

static const GZIID GZIID_cIGZPersistResourceKeyList = 0x656B8F12;

struct cGZPersistResourceKey;

class cIGZPersistResourceKeyList : public cIGZUnknown
{
public:
	typedef void (*tKeyListIterator)(cGZPersistResourceKey const& key, void* context);

public:
	virtual bool Insert(cGZPersistResourceKey const& key) = 0;
	virtual bool Insert(cIGZPersistResourceKeyList const& list) = 0;

	virtual bool Erase(cGZPersistResourceKey const& key) = 0;
	virtual bool EraseAll(void) = 0;

	virtual void EnumKeys(tKeyListIterator fn, void* context) const = 0;

	virtual bool IsPresent(cGZPersistResourceKey const& key) = 0;
	virtual uint32_t Size(void) const = 0;
	virtual cGZPersistResourceKey GetKey(uint32_t other) const = 0;
};