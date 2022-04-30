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

static const GZIID GZIID_cIGZMessageServer2 = 0x652294C7;

class cIGZMessage2;
class cIGZMessageTarget2;

class cIGZMessageServer2 : public cIGZUnknown
{
public:
	virtual bool MessageSend(cIGZMessage2* msg) = 0;
	virtual bool MessagePost(cIGZMessage2* msg, bool pushFront) = 0;

	virtual bool AddNotification(cIGZMessageTarget2* target, GZGUID msgType) = 0;
	virtual bool RemoveNotification(cIGZMessageTarget2* target, GZGUID msgType) = 0;

	virtual void GeneralMessagePostToTarget(cIGZMessage2* msg, cIGZMessageTarget2* target) = 0;
	virtual void CancelGeneralMessagePostsToTarget(cIGZMessageTarget2* target) = 0;

	virtual bool OnTick(uint32_t tickCount) = 0;

	virtual int32_t GetMessageQueueSize() = 0;

	virtual void SetAlwaysClearQueueOnTick(bool clearOnTick) = 0;

	virtual uint32_t GetRefCount() const = 0;

	virtual bool CreateMessage(GZCLSID clsid, GZIID iid, void** msgOut) = 0;
};