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

#include "cGZPersistResourceKey.h"
#include "cGZPersistResourceKeyFilter.h"

cGZPersistResourceKeyFilter::cGZPersistResourceKeyFilter(void) :
	refCount(0)
{
}

bool cGZPersistResourceKeyFilter::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistResourceKeyFilter)
	{
		*outPtr = static_cast<cIGZPersistResourceKeyFilter*>(this);
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

uint32_t cGZPersistResourceKeyFilter::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZPersistResourceKeyFilter::Release(void)
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

bool cGZPersistResourceKeyFilter::IsKeyIncluded(cGZPersistResourceKey const& key)
{
	return true;
}