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
	data(NULL),
	length(0)
{
}

bool cGZPersistBinaryResource::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistBinaryResource)
	{
		*outPtr = static_cast<cIGZPersistBinaryResource*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cGZPersistResource::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZPersistBinaryResource::AddRef(void)
{
	return cGZPersistResource::AddRef();
}

uint32_t cGZPersistBinaryResource::Release(void)
{
	return cGZPersistResource::Release();
}

void cGZPersistBinaryResource::SetData(void* newData, uint32_t newLength)
{
	if (newData == NULL || newLength == 0)
	{
		if (data != NULL)
		{
			delete[] data;
			data = NULL;
		}

		allocatedLength = 0;
		length = 0;
	}
	else
	{
		void* targetBuffer;
		if (allocatedLength < newLength)
		{
			allocatedLength = newLength;
			if (data != NULL)
			{
				delete[] data;
			}

			targetBuffer = new uint8_t[allocatedLength];
			data = targetBuffer;
		}
		else
		{
			targetBuffer = data;
		}

		length = newLength;
		if (newLength < 32)
		{
			memcpy(targetBuffer, newData, newLength);
		}
		else if (newLength > 0)
		{
			memmove(targetBuffer, newData, newLength);
		}
	}
}

void* cGZPersistBinaryResource::GetData(void) const
{
	return data;
}

uint32_t cGZPersistBinaryResource::GetDataLength(void) const
{
	return length;
}