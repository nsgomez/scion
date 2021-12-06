#pragma once
#include <stdint.h>
#include "GZTypes.h"

static const GZREFIID GZIID_cIGZUnknown = 0x00000001;

class cIGZUnknown
{
public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr) = 0;
	virtual uint32_t AddRef() = 0;
	virtual uint32_t Release() = 0;
};