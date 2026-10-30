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

#include "cIGZString.h"
#include "cRZMutex.h"

cRZMutex::cRZMutex(cIGZString const& mutexName) :
	mMutex(NULL)
{
	MutexPlatformData* data = new MutexPlatformData();
	mMutex = data;

	data->mName = mutexName;
	data->mHandle = CreateMutex(NULL, FALSE, mutexName.ToChar());
	data->mValid = data->mHandle != NULL;
}

cRZMutex::~cRZMutex()
{
	MutexPlatformData* data = mMutex;
	CloseHandle(data->mHandle);
	delete data;
}

uint32_t cRZMutex::Release()
{
	if (this)
	{
		delete this;
	}

	return 0;
}

uint32_t cRZMutex::Lock()
{
	return WaitForSingleObject(mMutex->mHandle, INFINITE) != WAIT_FAILED;
}

uint32_t cRZMutex::Unlock()
{
	return ReleaseMutex(mMutex->mHandle) != 0;
}

uint32_t cRZMutex::TryLock()
{
	DWORD result = WaitForSingleObject(mMutex->mHandle, 0);
	return result != WAIT_FAILED && result != WAIT_TIMEOUT;
}

uint32_t cRZMutex::IsValid()
{
	return mMutex->mValid;
}

uint32_t cRZMutex::GetMutexHandle(HANDLE* handleOut)
{
	*handleOut = mMutex->mHandle;
	return 1;
}