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

#include <Windows.h>
#include "cRZTimer.h"
#include "cRZTimerConstants.h"

static cRZTimerConstants sTimerConstants;

cRZTimer::Units cRZTimer::GetUnits() const
{
	return mTimerUnit;
}

void cRZTimer::SetUnits(cRZTimer::Units units)
{
	mTimerUnit = units;
	mStartTime = 0;
	mElapsedTime = 0;
}

bool cRZTimer::IsTimerRunning() const
{
	return mStartTime != 0;
}

void cRZTimer::Reset()
{
	mStartTime = 0;
	mElapsedTime = 0;
}

void cRZTimer::Restart()
{
	if (mTimerUnit == kTimerUnitCPUCycles)
	{
		mStartTime = rdtsc();
	}
	else
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mStartTime));
	}

	mElapsedTime = 0;
}

void cRZTimer::Start()
{
	if (mTimerUnit == kTimerUnitCPUCycles)
	{
		mStartTime = rdtsc();
	}
	else
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mStartTime));
	}
}

void cRZTimer::Stop()
{
	if (mStartTime == 0)
	{
		return;
	}

	uint64_t now;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));

	int64_t delta = (int64_t)(now - mStartTime);
	if (delta > sTimerConstants.overheadTicks)
	{
		mElapsedTime += delta - sTimerConstants.overheadTicks;
	}
	else
	{
		mElapsedTime++;
	}

	mStartTime = 0;
}

void cRZTimer::Delay(uint32_t milliseconds)
{
	cRZTimer timer(mTimerUnit);
	timer.Start();

	while (timer.GetElapsedTime() < milliseconds)
	{
		Sleep(0);
	}
}

uint32_t cRZTimer::GetElapsedTime() const
{
	int64_t elapsed = mElapsedTime;
	if (mStartTime != 0)
	{
		uint64_t now;
		if (mTimerUnit == kTimerUnitCPUCycles)
		{
			now = rdtsc();
		}
		else
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		}

		int64_t delta = (int64_t)(now - mStartTime);
		if (delta > sTimerConstants.overheadTicks)
		{
			elapsed += delta - sTimerConstants.overheadTicks;
		}
		else
		{
			elapsed++;
		}
	}

	uint32_t elapsedTimeOut;
	switch (mTimerUnit)
	{
	case kTimerUnitCPUCycles:
	case kTimerUnitOSTicks:
		elapsedTimeOut = (uint32_t)elapsed;
		break;

	case kTimerUnitNanoseconds:
		elapsedTimeOut = (uint32_t)(elapsed * sTimerConstants.nanosecondsPerTick);
		break;

	case kTimerUnitMicroseconds:
		elapsedTimeOut = (uint32_t)(elapsed * sTimerConstants.microsecondsPerTick);
		break;

	case kTimerUnitMilliseconds:
		elapsedTimeOut = (uint32_t)(elapsed * sTimerConstants.millisecondsPerTick);
		break;

	case kTimerUnitSeconds:
		elapsedTimeOut = (uint32_t)(elapsed * sTimerConstants.secondsPerTick);
		break;

	case kTimerUnitMinutes:
		elapsedTimeOut = (uint32_t)(elapsed * sTimerConstants.minutesPerTick);
		break;

	default:
		elapsedTimeOut = 0;
		break;
	}

	return elapsedTimeOut;
}

void cRZTimer::GetElapsedTime(uint64_t& elapsedTimeOut) const
{
	int64_t elapsed = mElapsedTime;
	if (mStartTime != 0)
	{
		uint64_t now;
		if (mTimerUnit == kTimerUnitCPUCycles)
		{
			now = rdtsc();
		}
		else
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		}

		int64_t delta = (int64_t)(now - mStartTime);
		if (delta > sTimerConstants.overheadTicks)
		{
			elapsed += delta - sTimerConstants.overheadTicks;
		}
		else
		{
			elapsed++;
		}
	}

	switch (mTimerUnit)
	{
	case kTimerUnitCPUCycles:
	case kTimerUnitOSTicks:
		elapsedTimeOut = (uint64_t)elapsed;
		break;

	case kTimerUnitNanoseconds:
		elapsedTimeOut = (uint64_t)(elapsed * sTimerConstants.nanosecondsPerTick);
		break;

	case kTimerUnitMicroseconds:
		elapsedTimeOut = (uint64_t)(elapsed * sTimerConstants.microsecondsPerTick);
		break;

	case kTimerUnitMilliseconds:
		elapsedTimeOut = (uint64_t)(elapsed * sTimerConstants.millisecondsPerTick);
		break;

	case kTimerUnitSeconds:
		elapsedTimeOut = (uint64_t)(elapsed * sTimerConstants.secondsPerTick);
		break;

	case kTimerUnitMinutes:
		elapsedTimeOut = (uint64_t)(elapsed * sTimerConstants.minutesPerTick);
		break;

	default:
		elapsedTimeOut = 0;
		break;
	}
}

void cRZTimer::GetElapsedTime(float& elapsedTimeOut) const
{
	int64_t elapsed = mElapsedTime;
	if (mStartTime != 0)
	{
		uint64_t now;
		if (mTimerUnit == kTimerUnitCPUCycles)
		{
			now = rdtsc();
		}
		else
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		}

		int64_t delta = (int64_t)(now - mStartTime);
		if (delta > sTimerConstants.overheadTicks)
		{
			elapsed += delta - sTimerConstants.overheadTicks;
		}
		else
		{
			elapsed++;
		}
	}

	switch (mTimerUnit)
	{
	case kTimerUnitCPUCycles:
	case kTimerUnitOSTicks:
		elapsedTimeOut = (float)elapsed;
		break;

	case kTimerUnitNanoseconds:
		elapsedTimeOut = elapsed * sTimerConstants.nanosecondsPerTick;
		break;

	case kTimerUnitMicroseconds:
		elapsedTimeOut = elapsed * sTimerConstants.microsecondsPerTick;
		break;

	case kTimerUnitMilliseconds:
		elapsedTimeOut = elapsed * sTimerConstants.millisecondsPerTick;
		break;

	case kTimerUnitSeconds:
		elapsedTimeOut = elapsed * sTimerConstants.secondsPerTick;
		break;

	case kTimerUnitMinutes:
		elapsedTimeOut = elapsed * sTimerConstants.minutesPerTick;
		break;

	default:
		elapsedTimeOut = 0.0f;
		break;
	}
}

void cRZTimer::SetElapsedTime(int64_t elapsedTime)
{
	switch (mTimerUnit)
	{
	case kTimerUnitCPUCycles:
	case kTimerUnitOSTicks:
		mElapsedTime = elapsedTime;
		break;

	case kTimerUnitNanoseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerNanosecond);
		break;

	case kTimerUnitMicroseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMicrosecond);
		break;

	case kTimerUnitMilliseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMillisecond);
		break;

	case kTimerUnitSeconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerSecond);
		break;

	case kTimerUnitMinutes:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMinute);
		break;

	default:
		break;
	}

	if (mTimerUnit == kTimerUnitCPUCycles)
	{
		mStartTime = rdtsc();
	}
	else
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mStartTime));
	}
}

void cRZTimer::SetElapsedTime(float elapsedTime)
{
	switch (mTimerUnit)
	{
	case kTimerUnitCPUCycles:
	case kTimerUnitOSTicks:
		mElapsedTime = (uint64_t)elapsedTime;
		break;

	case kTimerUnitNanoseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerNanosecond);
		break;

	case kTimerUnitMicroseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMicrosecond);
		break;

	case kTimerUnitMilliseconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMillisecond);
		break;

	case kTimerUnitSeconds:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerSecond);
		break;

	case kTimerUnitMinutes:
		mElapsedTime = (uint64_t)(elapsedTime * sTimerConstants.ticksPerMinute);
		break;

	default:
		break;
	}

	if (mTimerUnit == kTimerUnitCPUCycles)
	{
		mStartTime = rdtsc();
	}
	else
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mStartTime));
	}
}

float cRZTimer::GetNanosecondsPerTick()
{
	return sTimerConstants.nanosecondsPerTick;
}

float cRZTimer::GetMicrosecondsPerTick()
{
	return sTimerConstants.microsecondsPerTick;
}

float cRZTimer::GetMillisecondsPerTick()
{
	return sTimerConstants.millisecondsPerTick;
}

float cRZTimer::GetSecondsPerTick()
{
	return sTimerConstants.secondsPerTick;
}

float cRZTimer::GetMinutesPerTick()
{
	return sTimerConstants.minutesPerTick;
}

float cRZTimer::GetMillisecondsPerCycle()
{
	return sTimerConstants.millisecondsPerCycle;
}

int32_t cRZTimer::GetTimerCycle(uint64_t* cycleOut)
{
	return QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(cycleOut));
}