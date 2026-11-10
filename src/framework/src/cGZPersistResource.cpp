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
#include "cGZPersistResource.h"
#include "RZPlatform.h"

 cGZPersistResource::cGZPersistResource() :
	mRefCount(0),
	mUnused(0),
	mKey()
{
}

bool cGZPersistResource::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	case GZIID_cIGZPersistResource:
		*outPtr = static_cast<cIGZPersistResource*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZPersistResource::AddRef(void)
{
	return RZThreadSafeAdd(&mRefCount, 1);
}

uint32_t cGZPersistResource::Release(void)
{
	uint32_t newRefCount = RZThreadSafeAdd(&mRefCount, -1);
	if (newRefCount > 0)
	{
		return newRefCount;
	}

	delete this;
	return 0;
}

cGZPersistResourceKey cGZPersistResource::GetKey(void) const
{
	return mKey;
}

bool cGZPersistResource::SetKey(cGZPersistResourceKey const& other)
{
	mKey = other;
	return true;
}

uint32_t cGZPersistResource::GetType(void) const
{
	return mKey.type;
}

uint32_t cGZPersistResource::GetGroup(void) const
{
	return mKey.group;
}

uint32_t cGZPersistResource::GetInstance(void) const
{
	return mKey.instance;
}

bool cGZPersistResource::SetType(uint32_t value)
{
	mKey.type = value;
	return true;
}

bool cGZPersistResource::SetGroup(uint32_t value)
{
	mKey.group = value;
	return true;
}

bool cGZPersistResource::SetInstance(uint32_t value)
{
	mKey.instance = value;
	return true;
}

uint32_t cGZPersistResource::GetRefCount(void)
{
	return mRefCount;
}