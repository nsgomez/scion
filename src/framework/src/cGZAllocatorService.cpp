#include <cstdlib>
#include "cGZAllocatorService.h"

cGZAllocatorService::cGZAllocatorService(void)
	: cRZSystemService(RZSRVID_cGZAllocatorService, 9000000)
{
}

bool cGZAllocatorService::QueryInterface(GZREFIID iid, void** outPtr)
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