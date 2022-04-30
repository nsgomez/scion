/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cstddef>
#include "cRZCriticalSection.h"

cRZCriticalSection::cRZCriticalSection()
{
	data = new CriticalSectionPlatformData();
	InitializeCriticalSection(&data->criticalSection);
}

cRZCriticalSection::~cRZCriticalSection()
{
	DeleteCriticalSection(&data->criticalSection);
	delete data;
}

uint32_t cRZCriticalSection::Lock()
{
	EnterCriticalSection(&data->criticalSection);
	return ++data->lockCount;
}

uint32_t cRZCriticalSection::Unlock()
{
	LeaveCriticalSection(&data->criticalSection);
	return --data->lockCount;
}

uint32_t cRZCriticalSection::TryLock()
{
	// TODO: This API is only available on Windows XP and above, so the game tries
	// to dynamically load it and can fall back to EnterCriticalSection.
	/*if (!TryEnterCriticalSection(criticalSection))
	{
		return false;
	}*/

	EnterCriticalSection(&data->criticalSection);
	return ++data->lockCount;
}

bool cRZCriticalSection::IsValid()
{
	return true;
}

bool cRZCriticalSection::IsLocked()
{
	return data->lockCount > 0;
}

void cRZCriticalSection::GetCriticalSectionHandle(void* handleOut)
{
	*reinterpret_cast<CRITICAL_SECTION**>(handleOut) = &this->data->criticalSection;
}