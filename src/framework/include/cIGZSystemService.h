#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZSystemService = 0x287FB697;

class cIGZSystemService : public cIGZUnknown
{
public:
	virtual GZGUID GetServiceID() = 0;
	virtual void SetServiceID(GZGUID id) = 0;

	virtual int32_t GetServicePriority() = 0;

	virtual bool IsServiceRunning() = 0;
	virtual void SetServiceRunning(bool running) = 0;

	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;
	virtual bool OnTick(int32_t totalTickFrames) = 0;
	virtual bool OnIdle(int32_t totalIdleFrames) = 0;

	virtual int32_t GetServiceTickPriority()
	{
		return 0;
	}
};