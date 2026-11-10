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

#include "cGZPersistDBRecord.h"

cGZPersistDBRecord::cGZPersistDBRecord(cIGZPersistDBSegment* segment) :
	mRefCount(0),
	mAccessFlags(0),
	mKey(),
	mSegment(segment)
{
}

cGZPersistDBRecord::cGZPersistDBRecord(cGZPersistResourceKey const& key, cIGZPersistDBSegment* segment) :
	mRefCount(0),
	mAccessFlags(0),
	mKey(key),
	mSegment(segment)
{
}

bool cGZPersistDBRecord::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZPersistDBRecord:
		*outPtr = static_cast<cIGZPersistDBRecord*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZPersistDBRecord::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZPersistDBRecord::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}
	else
	{
		delete this;
		return 0;
	}
}

cGZPersistResourceKey cGZPersistDBRecord::GetKey(void)
{
	return mKey;
}

int32_t cGZPersistDBRecord::GetAccessFlags(void)
{
	return mAccessFlags;
}

bool cGZPersistDBRecord::Close(void)
{
	bool succeeded = false;
	if (mSegment != NULL)
	{
		succeeded = mSegment->CloseRecord(this);
		mSegment = NULL;
	}

	return succeeded;
}

void cGZPersistDBRecord::SetKey(cGZPersistResourceKey const& key)
{
	mKey = key;
}

void cGZPersistDBRecord::SetAccessFlags(int32_t flags)
{
	mAccessFlags = flags;
}