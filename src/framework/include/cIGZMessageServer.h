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

static const GZIID GZIID_cIGZMessageServer = 0xC1085722;

struct cGZMessage;
class cIGZMessageTarget;

class cIGZMessageServer : public cIGZUnknown
{
public:
	virtual bool MessageSend(cGZMessage const& msg) = 0;
	virtual bool MessagePost(cGZMessage const& msg, bool pushFront) = 0;

	virtual bool AddNotification(cIGZMessageTarget* target, GZGUID msgType) = 0;
	virtual bool RemoveNotification(cIGZMessageTarget* target, GZGUID msgType) = 0;

	virtual void GeneralMessagePostToTarget(cGZMessage const& msg, cIGZMessageTarget* target) = 0;
	virtual void CancelGeneralMessagePostsToTarget(cIGZMessageTarget* target) = 0;

	virtual bool OnTick(uint32_t tickCount) = 0;

	virtual int32_t GetMessageQueueSize() = 0;

	virtual void SetAlwaysClearQueueOnTick(bool clearOnTick) = 0;
};