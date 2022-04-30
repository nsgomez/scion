/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cRZThreadSignal.h"

cRZThreadSignal::cRZThreadSignal(bool manualReset)
{
	SignalPlatformData* newSignal = new SignalPlatformData();
	this->signal = newSignal;

	newSignal->isManualReset = manualReset;
	newSignal->event = CreateEvent(NULL, manualReset, FALSE, NULL);
}

cRZThreadSignal::~cRZThreadSignal()
{
	CloseHandle(signal->event);
	delete signal;
}

uint32_t cRZThreadSignal::Release()
{
	if (this != NULL)
	{
		delete this;
	}

	return 0;
}

bool cRZThreadSignal::Signal()
{
	HANDLE hEvent = signal->event;
	bool result = false;

	if (!signal->isManualReset)
	{
		result = PulseEvent(hEvent) != FALSE;
	}
	else
	{
		SetEvent(hEvent);
	}

	return result;
}

bool cRZThreadSignal::Broadcast()
{
	return PulseEvent(signal->event) != FALSE;
}

cIGZThreadSignal::TimedWaitResult cRZThreadSignal::TimedWait(uint32_t waitTimeInMicroseconds)
{
	HANDLE hEvent = signal->event;

	cRZCriticalSectionHolder lock(signal->criticalSection);
	cIGZThreadSignal::TimedWaitResult operationResult = cIGZThreadSignal::TimedWaitResult::Success;

	if (waitTimeInMicroseconds != -1)
	{
		waitTimeInMicroseconds /= 1000;
	}

	DWORD waitResult = WaitForSingleObject(hEvent, waitTimeInMicroseconds /* converted to milliseconds */);

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

bool cRZThreadSignal::IsValid()
{
	return signal->event != NULL;
}

bool cRZThreadSignal::Unsignal()
{
	return ResetEvent(signal->event) != FALSE;
}

bool cRZThreadSignal::GetEventHandle(void* eventOut)
{
	*reinterpret_cast<HANDLE*>(eventOut) = signal->event;
	return true;
}