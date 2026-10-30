/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <cstddef>
#include <cstdlib>
#include "cRZCriticalSection.h"

cRZCriticalSection::cRZCriticalSection() :
	mPlatformData(NULL)
{
	mPlatformData = new CriticalSectionPlatformData();
	InitializeCriticalSection(&mPlatformData->mCriticalSection);
}

cRZCriticalSection::~cRZCriticalSection()
{
	CriticalSectionPlatformData* data = mPlatformData;
	DeleteCriticalSection(&data->mCriticalSection);
	delete data;
}

uint32_t cRZCriticalSection::Release()
{
	if (this)
	{
		delete this;
	}

	return 0;
}

uint32_t cRZCriticalSection::Lock()
{
	CriticalSectionPlatformData* data = mPlatformData;
	EnterCriticalSection(&data->mCriticalSection);
	return ++data->mLockCount;
}

uint32_t cRZCriticalSection::Unlock()
{
	CriticalSectionPlatformData* data = mPlatformData;
	const uint32_t lockCount = --data->mLockCount;
	LeaveCriticalSection(&data->mCriticalSection);
	return lockCount;
}

uint32_t cRZCriticalSection::TryLock()
{
	CriticalSectionPlatformData* data = mPlatformData;
	if (gTryEnterCriticalSection != NULL && !gTryEnterCriticalSection(&data->mCriticalSection))
	{
		return 0;
	}

	EnterCriticalSection(&data->mCriticalSection);
	return ++data->mLockCount;
}

uint32_t cRZCriticalSection::IsValid()
{
	return 1;
}

uint32_t cRZCriticalSection::IsLocked()
{
	return mPlatformData->mLockCount;
}

uint32_t cRZCriticalSection::GetCriticalSectionHandle(void* handleOut)
{
	*reinterpret_cast<CRITICAL_SECTION**>(handleOut) = &mPlatformData->mCriticalSection;
	return 1;
}