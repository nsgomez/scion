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
#include "cIGZPersistResourceKeyFilter.h"

class cGZPersistResourceKeyFilter : public cIGZPersistResourceKeyFilter
{
public:
	cGZPersistResourceKeyFilter(void);
	virtual ~cGZPersistResourceKeyFilter(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual bool IsKeyIncluded(cGZPersistResourceKey const& key);

protected:
	uint32_t refCount;
};