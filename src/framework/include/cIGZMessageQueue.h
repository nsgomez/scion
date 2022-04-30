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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZMessageQueue = 0x298B2F5E;

struct cGZMessage;

class cIGZMessageQueue : public cIGZUnknown
{
public:
	virtual void PushBack(cGZMessage const& msg) = 0;
	virtual void PushFront(cGZMessage const& msg) = 0;

	virtual bool RemoveFirstFound(cGZMessage const& msg) = 0;
	virtual bool RemoveLastFound(cGZMessage const& msg) = 0;
	virtual int32_t RemoveAllFound(cGZMessage const& msg) = 0;

	virtual bool RemoveFirstFound(GZGUID msgType) = 0;
	virtual bool RemoveLastFound(GZGUID msgType) = 0;
	virtual int32_t RemoveAllFound(GZGUID msgType) = 0;

	virtual int32_t RemoveAll() = 0;

	virtual GZGUID PeekFrontType() = 0;
	virtual GZGUID PeekBackType() = 0;
	virtual bool PeekAnywhereType(GZGUID msgType) = 0;

	// FUTURE: PeekFrontMessage and PeekBackMessage are inherently unsafe here
	virtual cGZMessage const* PeekFrontMessage() = 0;
	virtual cGZMessage const* PeekBackMessage() = 0;
	virtual bool PeekAnywhereMessage(cGZMessage const& msg) = 0;

	virtual GZGUID GetFrontType() = 0;
	virtual GZGUID GetBackType() = 0;
	virtual bool GetAnywhereType(GZGUID msgType) = 0;

	virtual bool GetFrontMessage(cGZMessage& msgOut) = 0;
	virtual bool GetBackMessage(cGZMessage& msgOut) = 0;
	virtual bool GetAnywhereMessage(cGZMessage const& msg) = 0;

	virtual GZGUID GetFrontTypeBlock() = 0;
	virtual GZGUID GetBackTypeBlock() = 0;

	virtual bool GetFrontMessageBlock(cGZMessage& msgOut) = 0;
	virtual bool GetBackMessageBlock(cGZMessage& msgOut) = 0;

	virtual int32_t GetMessageCount() = 0;
};