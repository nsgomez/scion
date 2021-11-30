#pragma once
#include "cIGZSystemService.h"
#include "cRZUnknown.h"

class cRZSystemService : public cIGZSystemService, public cRZUnknown
{
public:
	cRZSystemService(GZGUID serviceId, int32_t servicePriority);
	cRZSystemService(GZGUID serviceId, int32_t servicePriority, int32_t tickPriority);
	~cRZSystemService() { }

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual GZGUID GetServiceID(void);
	virtual void SetServiceID(GZGUID id);

	virtual int32_t GetServicePriority(void);

	virtual bool IsServiceRunning(void);
	virtual void SetServiceRunning(bool running);

	virtual bool Init(void);
	virtual bool Shutdown(void);
	virtual bool OnTick(void);
	virtual bool OnIdle(void);

	virtual int32_t GetServiceTickPriority(void);

protected:
	GZGUID serviceId;
	int32_t servicePriority;
	int32_t tickPriority;
	bool running;
};