#pragma once
#include "cIGZAllocatorService.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZAllocatorService = 0x3AE4BEA3;

class cGZAllocatorService : public cIGZAllocatorService, public cRZSystemService
{
public:
	cGZAllocatorService(void);
	virtual ~cGZAllocatorService(void) { }

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual void* Allocate(uint32_t size);
	virtual void Deallocate(void* data);
	virtual void* Reallocate(void* data, uint32_t newSize);
};