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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cIGZString.h"
#include "cRZMutex.h"

cRZMutex::cRZMutex(cIGZString const& mutexName)
{
	mutex = CreateMutex(NULL, false, mutexName.ToChar());
}

cRZMutex::~cRZMutex()
{
	CloseHandle(mutex);
}

void cRZMutex::Release()
{
	if (this)
	{
		delete this;
	}
}

bool cRZMutex::Lock()
{
	return WaitForSingleObject(mutex, -1) != WAIT_FAILED;
}

bool cRZMutex::Unlock()
{
	return ReleaseMutex(mutex) != 0;
}

bool cRZMutex::TryLock()
{
	DWORD result = WaitForSingleObject(mutex, 0);
	return result != WAIT_FAILED && result != WAIT_TIMEOUT;
}

bool cRZMutex::IsValid()
{
	return mutex != NULL;
}

HANDLE cRZMutex::GetMutexHandle()
{
	return mutex;
}