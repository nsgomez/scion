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