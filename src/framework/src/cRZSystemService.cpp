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

#include "cRZSystemService.h"

cRZSystemService::cRZSystemService(GZGUID serviceId, int32_t servicePriority)
{
	this->serviceId = serviceId;
	this->servicePriority = servicePriority;
	this->tickPriority = servicePriority;
	this->running = false;
}

cRZSystemService::cRZSystemService(GZGUID serviceId, int32_t servicePriority, int32_t tickPriority)
{
	this->serviceId = serviceId;
	this->servicePriority = servicePriority;
	this->tickPriority = tickPriority;
	this->running = false;
}

bool cRZSystemService::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZSystemService)
	{
		*outPtr = static_cast<cIGZSystemService*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZUnknown::QueryInterface(iid, outPtr);
	}
}

uint32_t cRZSystemService::AddRef()
{
	return cRZUnknown::AddRef();
}

uint32_t cRZSystemService::Release()
{
	return cRZUnknown::Release();
}

GZGUID cRZSystemService::GetServiceID()
{
	return serviceId;
}

void cRZSystemService::SetServiceID(GZGUID id)
{
	this->serviceId = id;
}

int32_t cRZSystemService::GetServicePriority()
{
	return servicePriority;
}

bool cRZSystemService::IsServiceRunning()
{
	return running;
}

void cRZSystemService::SetServiceRunning(bool running)
{
	this->running = running;
}

bool cRZSystemService::Init()
{
	return true;
}

bool cRZSystemService::Shutdown()
{
	return true;
}

bool cRZSystemService::OnTick(int32_t totalTickFrames)
{
	return true;
}

bool cRZSystemService::OnIdle(int32_t totalIdleFrames)
{
	return true;
}

int32_t cRZSystemService::GetServiceTickPriority()
{
	return tickPriority;
}