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

static const GZIID GZIID_cIGZMessageQueue2 = 0xA52294CD;

class cIGZMessage2;

class cIGZMessageQueue2 : public cIGZUnknown
{
public:
	virtual void PushBack(cIGZMessage2* msg) = 0;
	virtual void PushFront(cIGZMessage2* msg) = 0;

	virtual bool RemoveFirstFound(cIGZMessage2 const& msg) = 0;
	virtual bool RemoveLastFound(cIGZMessage2 const& msg) = 0;
	virtual int32_t RemoveAllFound(cIGZMessage2 const& msg) = 0;

	virtual bool RemoveFirstFound(GZGUID msgType) = 0;
	virtual bool RemoveLastFound(GZGUID msgType) = 0;
	virtual int32_t RemoveAllFound(GZGUID msgType) = 0;

	virtual int32_t RemoveAll() = 0;

	virtual GZGUID PeekFrontType() = 0;
	virtual GZGUID PeekBackType() = 0;
	virtual bool PeekAnywhereType(GZGUID msgType) = 0;

	virtual bool PeekFrontMessage(cIGZMessage2** msgOut) = 0;
	virtual bool PeekBackMessage(cIGZMessage2** msgOut) = 0;
	virtual bool PeekAnywhereMessage(cIGZMessage2 const& msgIn, cIGZMessage2** msgOut) = 0;

	virtual bool GetFrontMessage(cIGZMessage2** msgOut) = 0;
	virtual bool GetBackMessage(cIGZMessage2** msgOut) = 0;

	virtual bool GetFrontMessageBlock(cIGZMessage2** msgOut) = 0;
	virtual bool GetBackMessageBlock(cIGZMessage2** msgOut) = 0;

	virtual int32_t GetMessageCount() = 0;
};