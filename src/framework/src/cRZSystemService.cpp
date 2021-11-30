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

uint32_t cRZSystemService::AddRef(void)
{
	return cRZUnknown::AddRef();
}

uint32_t cRZSystemService::Release(void)
{
	return cRZUnknown::Release();
}

GZGUID cRZSystemService::GetServiceID(void)
{
	return serviceId;
}

void cRZSystemService::SetServiceID(GZGUID id)
{
	this->serviceId = id;
}

int32_t cRZSystemService::GetServicePriority(void)
{
	return servicePriority;
}

bool cRZSystemService::IsServiceRunning(void)
{
	return running;
}

void cRZSystemService::SetServiceRunning(bool running)
{
	this->running = running;
}

bool cRZSystemService::Init(void)
{
	return true;
}

bool cRZSystemService::Shutdown(void)
{
	return true;
}

bool cRZSystemService::OnTick(void)
{
	return true;
}

bool cRZSystemService::OnIdle(void)
{
	return true;
}

int32_t cRZSystemService::GetServiceTickPriority(void)
{
	return tickPriority;
}