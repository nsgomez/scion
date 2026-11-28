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

cRZCallbackTimer::cRZCallbackTimer()
{
}

cRZCallbackTimer::~cRZCallbackTimer()
{
}

bool cRZCallbackTimer::SetResolution(uint32_t resolutionInMs)
{
	return false;
}

bool cRZCallbackTimer::GetResolution(uint32_t& resolutionInMs)
{
	return false;
}

bool cRZCallbackTimer::SetPeriod(uint32_t periodInMs)
{
	return false;
}

bool cRZCallbackTimer::GetPeriod(uint32_t& resolutionInMs)
{
	return false;
}

bool cRZCallbackTimer::StartPeriodic(uint32_t periodInMs, void* context)
{
	return false;
}

bool cRZCallbackTimer::StartOneShot(uint32_t delayInMs, void* context)
{
	return false;
}

bool cRZCallbackTimer::Stop()
{
	return false;
}

bool cRZCallbackTimer::IsRunning()
{
	return mIsRunning;
}

bool cRZCallbackTimer::SetCallbackFunction(void (*callback)(uint32_t context), uint32_t context)
{
	return false;
}

bool cRZCallbackTimer::SetCallbackFunction(void (*callback)(void* context), void* context)
{
	return false;
}

bool cRZCallbackTimer::EnableSharedTimerUse(bool isEnabled)
{
	return false;
}