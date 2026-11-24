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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZMessageQueueManager2 = 0x652294BC;

class cIGZMessageQueueManager2 : public cIGZUnknown
{
public:
	virtual bool GetQueue(GZGUID id, cIGZMessageQueue2** queueOut) = 0;

	virtual bool AddQueue(GZGUID id) = 0;
	virtual bool AddQueue(GZGUID id, cIGZMessageQueue2* queue) = 0;

	virtual bool RemoveQueue(GZGUID id) = 0;
	virtual bool RemoveQueue(cIGZMessageQueue2* queue) = 0;
	virtual bool RemoveAllQueues() = 0;

	virtual uint32_t GetQueueCount() = 0;
};