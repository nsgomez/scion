#pragma once
#include <stdint.h>
#include "GZTypes.h"

static const GZREFIID GZIID_cIGZUnknown = 1;

class cIGZUnknown
{
public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr) = 0;
	virtual uint32_t AddRef(void) = 0;
	virtual uint32_t Release(void) = 0;
};