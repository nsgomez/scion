/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include <cstdlib>
#include "cGZAllocatorService.h"

cGZAllocatorService::cGZAllocatorService(void)
	: cRZSystemService(RZSRVID_cGZAllocatorService, 9000000)
{
}

bool cGZAllocatorService::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZAllocatorService)
	{
		*outPtr = static_cast<cIGZAllocatorService*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZAllocatorService::AddRef(void)
{
	return cRZSystemService::AddRef();
}

uint32_t cGZAllocatorService::Release(void)
{
	return cRZSystemService::Release();
}

void* cGZAllocatorService::Allocate(uint32_t size)
{
	return malloc(size);
}

void cGZAllocatorService::Deallocate(void* data)
{
	free(data);
}

void* cGZAllocatorService::Reallocate(void* data, uint32_t newSize)
{
	return realloc(data, newSize);
}