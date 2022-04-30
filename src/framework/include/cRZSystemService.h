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
#include "cIGZSystemService.h"
#include "cRZUnknown.h"

class cRZSystemService : public cIGZSystemService, public cRZUnknown
{
public:
	cRZSystemService(GZGUID serviceId, int32_t servicePriority);
	cRZSystemService(GZGUID serviceId, int32_t servicePriority, int32_t tickPriority);
	virtual ~cRZSystemService() { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual GZGUID GetServiceID();
	virtual void SetServiceID(GZGUID id);

	virtual int32_t GetServicePriority();

	virtual bool IsServiceRunning();
	virtual void SetServiceRunning(bool running);

	virtual bool Init();
	virtual bool Shutdown();
	virtual bool OnTick(int32_t totalTickFrames);
	virtual bool OnIdle(int32_t totalIdleFrames);

	virtual int32_t GetServiceTickPriority();

protected:
	GZGUID serviceId;
	int32_t servicePriority;
	int32_t tickPriority;
	bool running;
};