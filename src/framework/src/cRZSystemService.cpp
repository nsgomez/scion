#include "cRZSystemService.h"

cRZSystemService::cRZSystemService(GZGUID serviceId, int32_t servicePriority)
{
	this->serviceId = serviceId;
	this->servicePriority = servicePriority;
	this->tickPriority = servicePriority;
	this->running = false;
}

cRZSystemService::cRZSystemService(GZGUID serviceId, int32_t servicePriority, int32_t tickPriority)
{
	this->serviceId = serviceId;
	this->servicePriority = servicePriority;
	this->tickPriority = tickPriority;
	this->running = false;
}

bool cRZSystemService::QueryInterface(GZREFIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZSystemService)
	{
		*outPtr = static_cast<cIGZSystemService*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZUnknown::QueryInterface(iid, outPtr);
	}
}

uint32_t cRZSystemService::AddRef()
{
	return cRZUnknown::AddRef();
}

uint32_t cRZSystemService::Release()
{
	return cRZUnknown::Release();
}

GZGUID cRZSystemService::GetServiceID()
{
	return serviceId;
}

void cRZSystemService::SetServiceID(GZGUID id)
{
	this->serviceId = id;
}

int32_t cRZSystemService::GetServicePriority()
{
	return servicePriority;
}

bool cRZSystemService::IsServiceRunning()
{
	return running;
}

void cRZSystemService::SetServiceRunning(bool running)
{
	this->running = running;
}

bool cRZSystemService::Init()
{
	return true;
}

bool cRZSystemService::Shutdown()
{
	return true;
}

bool cRZSystemService::OnTick(int32_t totalTickFrames)
{
	return true;
}

bool cRZSystemService::OnIdle(int32_t totalIdleFrames)
{
	return true;
}

int32_t cRZSystemService::GetServiceTickPriority()
{
	return tickPriority;
}