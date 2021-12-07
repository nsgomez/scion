#pragma once
#include "cIGZSystemService.h"
#include "cRZUnknown.h"

class cRZSystemService : public cIGZSystemService, public cRZUnknown
{
public:
	cRZSystemService(GZGUID serviceId, int32_t servicePriority);
	cRZSystemService(GZGUID serviceId, int32_t servicePriority, int32_t tickPriority);
	virtual ~cRZSystemService() { }

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual GZGUID GetServiceID();
	virtual void SetServiceID(GZGUID id);

	virtual int32_t GetServicePriority();

	virtual bool IsServiceRunning();
	virtual void SetServiceRunning(bool running);

	virtual bool Init();
	virtual bool Shutdown();
	virtual bool OnTick(int32_t totalTickFrames);
	virtual bool OnIdle(int32_t totalIdleFrames);

	virtual int32_t GetServiceTickPriority();

protected:
	GZGUID serviceId;
	int32_t servicePriority;
	int32_t tickPriority;
	bool running;
};