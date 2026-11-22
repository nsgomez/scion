/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cRZCallbackTimer.h"

// TODO: everything in cRZCallbackTimer is currently just non-functional stubs to satisfy the linker

cRZCallbackTimer::cRZCallbackTimer()
{
}

cRZCallbackTimer::~cRZCallbackTimer()
{
}

bool cRZCallbackTimer::QueryInterface(GZIID iid, void** outPtr)
{
	return false;
}

uint32_t cRZCallbackTimer::AddRef()
{
	return 0;
}

uint32_t cRZCallbackTimer::Release()
{
	return 0;
}

bool cRZCallbackTimer::Init()
{
	return false;
}

bool cRZCallbackTimer::Shutdown()
{
	return false;
}

bool cRZCallbackTimer::Start(uint32_t intervalMs, void* context)
{
	return false;
}

bool cRZCallbackTimer::Cancel()
{
	return false;
}

bool cRZCallbackTimer::IsActive()
{
	return false;
}

bool cRZCallbackTimer::SetCallback(void (*callback)(void*), void* context)
{
	return false;
}

bool cRZCallbackTimer::Reset()
{
	return false;
}

uint32_t cRZCallbackTimer::Interval()
{
	return -1;
}