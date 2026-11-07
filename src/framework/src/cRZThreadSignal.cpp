/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cRZThreadSignal.h"

cRZThreadSignal::cRZThreadSignal(bool manualReset) :
	mSignal(NULL)
{
	SignalPlatformData* newSignal = new SignalPlatformData();
	mSignal = newSignal;

	newSignal->isManualReset = manualReset;

	if (!manualReset)
	{
		newSignal->event = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	else
	{
		newSignal->event = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
}

cRZThreadSignal::~cRZThreadSignal()
{
	SignalPlatformData* platformData = mSignal;
	CloseHandle(platformData->event);
	delete platformData;
}

uint32_t cRZThreadSignal::Release()
{
	if (this != NULL)
	{
		delete this;
	}

	return 0;
}

int32_t cRZThreadSignal::Signal()
{
	int32_t result = FALSE;
	if (!mSignal->isManualReset)
	{
		result = PulseEvent(mSignal->event) != FALSE;
	}
	else
	{
		SetEvent(mSignal->event);
	}

	return result;
}

bool cRZThreadSignal::Broadcast()
{
	return PulseEvent(mSignal->event) != FALSE;
}

cIGZThreadSignal::TimedWaitResult cRZThreadSignal::TimedWait(uint32_t waitTimeInMicroseconds)
{
	SignalPlatformData* platformData = mSignal;
	cRZCriticalSectionHolder lock(mSignal->criticalSection);

	cIGZThreadSignal::TimedWaitResult operationResult = cIGZThreadSignal::TimedWaitResult::Success;
	uint32_t waitTimeInMilliseconds = (waitTimeInMicroseconds == -1) ? waitTimeInMicroseconds : (waitTimeInMicroseconds / 1000);

	DWORD waitResult = WaitForSingleObject(mSignal->event, waitTimeInMilliseconds);
	if (waitResult == WAIT_FAILED)
	{
		operationResult = cIGZThreadSignal::TimedWaitResult::Failure;
	}
	else if (waitResult == WAIT_TIMEOUT)
	{
		operationResult = cIGZThreadSignal::TimedWaitResult::Timeout;
	}

	return operationResult;
}

int32_t cRZThreadSignal::IsValid()
{
	return mSignal->event != NULL;
}

bool cRZThreadSignal::Unsignal()
{
	return ResetEvent(mSignal->event) != FALSE;
}

int32_t cRZThreadSignal::GetEventHandle(void* eventOut)
{
	*reinterpret_cast<HANDLE*>(eventOut) = mSignal->event;
	return true;
}