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

#pragma once
#include "GZTypes.h"

class cIGZThreadSignal
{
public:
	enum TimedWaitResult
	{
		Failure,
		Success,
		Timeout,
	};

public:
	virtual uint32_t Release() = 0;
	virtual bool Signal() = 0;
	virtual bool Broadcast() = 0;
	virtual TimedWaitResult TimedWait(uint32_t waitTimeInMicroseconds) = 0;
	virtual bool IsValid() = 0;
};