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

#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZSystemService = 0x287FB697;

class cIGZSystemService : public cIGZUnknown
{
public:
	virtual GZGUID GetServiceID() = 0;
	virtual void SetServiceID(GZGUID id) = 0;

	virtual int32_t GetServicePriority() = 0;

	virtual bool IsServiceRunning() = 0;
	virtual void SetServiceRunning(bool running) = 0;

	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;
	virtual bool OnTick(int32_t totalTickFrames) = 0;
	virtual bool OnIdle(int32_t totalIdleFrames) = 0;

	virtual int32_t GetServiceTickPriority()
	{
		return 0;
	}
};