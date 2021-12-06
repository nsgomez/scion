#include <cstddef>
#include "cRZCriticalSection.h"

cRZCriticalSection::cRZCriticalSection()
{
	criticalSection = new CRITICAL_SECTION();
	InitializeCriticalSection(criticalSection);
}

cRZCriticalSection::~cRZCriticalSection()
{
	DeleteCriticalSection(criticalSection);
}

void cRZCriticalSection::Release()
{
	if (this != NULL)
	{
		delete this;
	}
}

uint32_t cRZCriticalSection::Lock()
{
	EnterCriticalSection(criticalSection);
	return ++lockCount;
}

uint32_t cRZCriticalSection::Unlock()
{
	LeaveCriticalSection(criticalSection);
	return --lockCount;
}

uint32_t cRZCriticalSection::TryLock()
{
	// TODO: This API is only available on Windows XP and above, so the game tries
	// to dynamically load it and can fall back to EnterCriticalSection.
	/*if (!TryEnterCriticalSection(criticalSection))
	{
		return false;
	}*/

	EnterCriticalSection(criticalSection);
	return ++lockCount;
}

bool cRZCriticalSection::IsValid()
{
	return true;
}

bool cRZCriticalSection::IsLocked()
{
	return lockCount > 0;
}

void cRZCriticalSection::GetCriticalSectionHandle(CRITICAL_SECTION** handleOut)
{
	*handleOut = this->criticalSection;
}