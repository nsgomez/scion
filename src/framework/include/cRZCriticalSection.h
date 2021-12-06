#pragma once
#include <stdint.h>
#include <Windows.h>

class cRZCriticalSection
{
public:
	cRZCriticalSection();
	virtual ~cRZCriticalSection();

public:
	virtual void Release();
	virtual uint32_t Lock();
	virtual uint32_t Unlock();
	virtual uint32_t TryLock();
	virtual bool IsValid();
	virtual bool IsLocked();

protected:
	virtual void GetCriticalSectionHandle(CRITICAL_SECTION** handleOut);

private:
	CRITICAL_SECTION* criticalSection;
	uint32_t lockCount;
};