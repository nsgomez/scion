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
#include "GZTypes.h"

class cRZCallbackTimer
{
public:
	cRZCallbackTimer();
	virtual ~cRZCallbackTimer();

public:
	virtual bool SetResolution(uint32_t resolutionInMs);
	virtual bool GetResolution(uint32_t& resolutionInMs);

	virtual bool SetPeriod(uint32_t periodInMs);
	virtual bool GetPeriod(uint32_t& resolutionInMs);

	virtual bool StartPeriodic(uint32_t periodInMs, void* context);
	virtual bool StartOneShot(uint32_t delayInMs, void* context);
	virtual bool Stop();
	virtual bool IsRunning();

	virtual bool SetCallbackFunction(void (*callback)(uint32_t context), uint32_t context);
	virtual bool SetCallbackFunction(void (*callback)(void* context), void* context);

	virtual bool EnableSharedTimerUse(bool isEnabled);

protected:
	bool mRegistered;
	uint32_t mUnknown8;
	uint32_t mUnknownC;
	uint32_t mResolutionInMs;
	uint32_t mPeriodInMs;
	bool mIsRunning;
	bool mUnknown19;
	uint32_t mTimerHandle;
	void* mTimerService;
	void* mContext;
};