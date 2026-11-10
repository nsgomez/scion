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

#include "cGZPersistBinaryResource.h"

cGZPersistBinaryResource::cGZPersistBinaryResource() :
	cGZPersistResource(),
	mData(NULL),
	mLength(0)
{
}

cGZPersistBinaryResource::~cGZPersistBinaryResource()
{
	SetData(NULL, 0);
}

bool cGZPersistBinaryResource::QueryInterface(GZIID iid, void** outPtr)
{
	*outPtr = NULL;

	if (iid != GZIID_cIGZPersistBinaryResource)
	{
		return cGZPersistResource::QueryInterface(iid, outPtr);
	}

	*outPtr = static_cast<cIGZPersistBinaryResource*>(this);
	AddRef();
	return true;
}

uint32_t cGZPersistBinaryResource::AddRef(void)
{
	return cGZPersistResource::AddRef();
}

uint32_t cGZPersistBinaryResource::Release(void)
{
	return cGZPersistResource::Release();
}

bool cGZPersistBinaryResource::SetData(void* newData, uint32_t newLength)
{
	if (newData == NULL || newLength == 0)
	{
		if (mData != NULL)
		{
			delete[] mData;
			mData = NULL;
		}

		mLength = 0;
		mAllocatedLength = 0;
	}
	else
	{
		if (mAllocatedLength < newLength)
		{
			mAllocatedLength = newLength;
			if (mData != NULL)
			{
				delete[] mData;
			}

			mData = new uint8_t[mAllocatedLength];
		}

		mLength = newLength;
		memcpy(mData, newData, newLength);
	}

	return true;
}

void* cGZPersistBinaryResource::GetData(void) const
{
	return mData;
}

uint32_t cGZPersistBinaryResource::GetDataLength(void) const
{
	return mLength;
}