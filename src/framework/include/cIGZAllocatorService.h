#pragma once
#include "cIGZSystemService.h"

static const GZREFIID GZIID_cIGZAllocatorService = 0x3AE4BEAB;

class cIGZAllocatorService : public cIGZSystemService
{
public:
	virtual void* Allocate(uint32_t size) = 0;
	virtual void Deallocate(void* data) = 0;
	virtual void* Reallocate(void* data, uint32_t newSize) = 0;
};