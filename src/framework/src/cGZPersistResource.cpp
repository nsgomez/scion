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
#include "cGZPersistResource.h"
#include "RZPlatform.h"

 cGZPersistResource::cGZPersistResource() :
	refCount(0),
	key()
{
}

bool cGZPersistResource::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZPersistResource)
	{
		*outPtr = static_cast<cIGZPersistResource*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistResource::AddRef(void)
{
	return RZThreadSafeAdd(&refCount, 1);
}

uint32_t cGZPersistResource::Release(void)
{
	int32_t newRefCount = RZThreadSafeAdd(&refCount, -1);
	if (newRefCount == 0)
	{
		delete this;
	}

	return newRefCount;
}

cGZPersistResourceKey cGZPersistResource::GetKey(void) const
{
	return key;
}

void cGZPersistResource::SetKey(cGZPersistResourceKey const& other)
{
	key = other;
}

uint32_t cGZPersistResource::GetType(void) const
{
	return key.type;
}

uint32_t cGZPersistResource::GetGroup(void) const
{
	return key.group;
}

uint32_t cGZPersistResource::GetInstance(void) const
{
	return key.instance;
}

void cGZPersistResource::SetType(uint32_t value)
{
	key.type = value;
}

void cGZPersistResource::SetGroup(uint32_t value)
{
	key.group = value;
}

void cGZPersistResource::SetInstance(uint32_t value)
{
	key.instance = value;
}

uint32_t cGZPersistResource::GetRefCount(void)
{
	return refCount;
}