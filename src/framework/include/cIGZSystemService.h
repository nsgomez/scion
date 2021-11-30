#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZSystemService = 0x287FB697;

class cIGZSystemService : public cIGZUnknown
{
public:
	virtual GZGUID GetServiceID(void) = 0;
	virtual void SetServiceID(GZGUID id) = 0;

	virtual int32_t GetServicePriority(void) = 0;

	virtual bool IsServiceRunning(void) = 0;
	virtual void SetServiceRunning(bool running) = 0;

	virtual bool Init(void) = 0;
	virtual bool Shutdown(void) = 0;
	virtual bool OnTick(void) = 0;
	virtual bool OnIdle(void) = 0;

	virtual int32_t GetServiceTickPriority(int)
	{
		return 0;
	}
};