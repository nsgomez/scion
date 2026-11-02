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

#include <stdint.h>
#include <Windows.h>
#include "cRZTimerConstants.h"

static const float kSecondsPerMinute = 1.0f / 60.0f;
static const float kMillisecondsPerSecond = 1000.0f;
static const float kMicrosecondsPerSecond = kMillisecondsPerSecond * 1000.0f;
static const float kNanosecondsPerSecond = kMicrosecondsPerSecond * 1000.0f;

static LARGE_INTEGER sTicksPerSecond;
static LARGE_INTEGER sCyclesPerSecond;

__forceinline uint64_t rdtsc()
{
	uint32_t lo, hi;
	__asm
	{
		cpuid // barrier to prevent the CPU from reordering rdtsc
		rdtsc
		mov lo, eax
		mov hi, edx
	}

	return (((uint64_t)hi) << 32) | lo;
}

static void MeasureFrequency(LARGE_INTEGER* tickFreqOut, LARGE_INTEGER* cycleFreqOut)
{
	cycleFreqOut->QuadPart = 0;

	// Try to measure timings using the ratio of the CPU timestamp counter to the OS performance counter.
	for (int i = 0; i < 5; i++)
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		// FUTURE: use intrinsics instead of inline assembly
		uint64_t cycles = rdtsc();
		double ratio = (double)cycles / (double)ticks.QuadPart;
		if (ratio > 512.19 && ratio < 2048.03)
		{
			QueryPerformanceFrequency(tickFreqOut);
			if (cycleFreqOut->QuadPart != 0)
			{
				return;
			}

			break;
		}
	}

	// If we didn't get a plausible result, measure the CPU TSC over a 0.33sec interval.
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER ticks1;
	QueryPerformanceCounter(&ticks1);
	uint64_t cycles1 = rdtsc();

	LARGE_INTEGER ticks2;
	uint64_t cycles2;

	do
	{
		SleepEx(100, TRUE);
		QueryPerformanceCounter(&ticks2);
		cycles2 = rdtsc();
	}
	while ((double)(ticks2.QuadPart - ticks2.QuadPart) / (double)freq.QuadPart < 0.33);

	double elapsedSeconds = (double)(ticks2.QuadPart - ticks1.QuadPart) / (double)freq.QuadPart;
	cycleFreqOut->QuadPart = (LONGLONG)((double)(cycles2 - cycles1) / elapsedSeconds);

	QueryPerformanceFrequency(tickFreqOut);
}

cRZTimerConstants::cRZTimerConstants()
{
	// Set the process to realtime priority to prevent timings from being messed up by the OS scheduler.
	HANDLE process = GetCurrentProcess();
	DWORD originalPrioClass = GetPriorityClass(process);

	HANDLE thread = GetCurrentThread();
	int originalThreadPrio = GetThreadPriority(thread);

	SetPriorityClass(process, REALTIME_PRIORITY_CLASS);
	SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);

	MeasureFrequency(&sTicksPerSecond, &sCyclesPerSecond);

	SetPriorityClass(process, originalPrioClass);
	SetThreadPriority(thread, originalThreadPrio);

	// Calculate tick rates
	secondsPerTick = 1.0f / (float)sTicksPerSecond.QuadPart;
	minutesPerTick = secondsPerTick * kSecondsPerMinute;
	millisecondsPerTick = secondsPerTick * kMillisecondsPerSecond;
	microsecondsPerTick = secondsPerTick * kMicrosecondsPerSecond;
	nanosecondsPerTick = secondsPerTick * kNanosecondsPerSecond;

	ticksPerNanosecond = 1.0f / nanosecondsPerTick;
	ticksPerMicrosecond = 1.0f / microsecondsPerTick;
	ticksPerMillisecond = 1.0f / millisecondsPerTick;
	ticksPerSecond = 1.0f / secondsPerTick;
	ticksPerMinute = 1.0f / minutesPerTick;

	// Measure overhead in ticks
	int64_t minDelta = 0x7FFFFFFFFFFFFFFFLL;
	for (int i = 0; i < 8; i++)
	{
		LARGE_INTEGER tick1, tick2;
		QueryPerformanceCounter(&tick1);
		QueryPerformanceCounter(&tick2);

		if (tick2.QuadPart - tick1.QuadPart < minDelta)
		{
			minDelta = tick2.QuadPart - tick1.QuadPart;
		}
	}

	overheadTicks = minDelta;

	// Calculate cycle rates
	secondsPerCycle = 1.0f / (float)sCyclesPerSecond.QuadPart;
	minutesPerCycle = secondsPerCycle * kSecondsPerMinute;
	millisecondsPerCycle = secondsPerCycle * kMillisecondsPerSecond;
	microsecondsPerCycle = secondsPerCycle * kMicrosecondsPerSecond;
	nanosecondsPerCycle = secondsPerCycle * kNanosecondsPerSecond;

	cyclesPerNanosecond = 1.0f / nanosecondsPerCycle;
	cyclesPerMicrosecond = 1.0f / microsecondsPerCycle;
	cyclesPerMillisecond = 1.0f / millisecondsPerCycle;
	cyclesPerSecond = 1.0f / secondsPerCycle;
	cyclesPerMinute = 1.0f / minutesPerCycle;

	// Measure overhead in cycles
	minDelta = 0x7FFFFFFFFFFFFFFFLL;
	for (int i = 0; i < 8; i++)
	{
		uint64_t cycle1 = rdtsc();
		uint64_t cycle2 = rdtsc();
		if ((int64_t)(cycle2 - cycle1) < minDelta)
		{
			minDelta = (int64_t)(cycle2 - cycle1);
		}
	}

	overheadCycles = minDelta;
}