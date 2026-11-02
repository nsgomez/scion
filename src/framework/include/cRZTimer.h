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

#pragma once
#include <stdint.h>

class cRZTimer
{
public:
	enum Units
	{
		kTimerUnitCPUCycles,
		kTimerUnitOSTicks,
		kTimerUnitNanoseconds,
		kTimerUnitMicroseconds,
		kTimerUnitMilliseconds,
		kTimerUnitSeconds,
		kTimerUnitMinutes,
	};

public:
	cRZTimer(Units timerUnit) :
		mTimerUnit(timerUnit),
		mStartTime(0),
		mElapsedTime(0)
	{
	}

public:
	Units GetUnits() const;
	void SetUnits(Units units);

	bool IsTimerRunning() const;
	void Reset();
	void Restart();
	void Start();
	void Stop();
	void Delay(uint32_t milliseconds);

	uint32_t GetElapsedTime() const;
	void GetElapsedTime(uint64_t& elapsedTimeOut) const;
	void GetElapsedTime(float& elapsedTimeOut) const;

	void SetElapsedTime(int64_t elapsedTime);
	void SetElapsedTime(float elapsedTime);

public:
	static float GetNanosecondsPerTick();
	static float GetMicrosecondsPerTick();
	static float GetMillisecondsPerTick();
	static float GetSecondsPerTick();
	static float GetMinutesPerTick();

	static float GetMillisecondsPerCycle();

	static int32_t GetTimerCycle(uint64_t* cycleOut);

protected:
	Units mTimerUnit;
	int64_t mStartTime;
	int64_t mElapsedTime;
};