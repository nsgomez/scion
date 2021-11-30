#include "cRZUnknown.h"

bool cRZUnknown::QueryInterface(GZREFIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
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

uint32_t cRZUnknown::AddRef(void)
{
	return ++refCount;
}

uint32_t cRZUnknown::Release(void)
{
	assert(refCount > 0);
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}