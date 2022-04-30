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
#include <list>
#include <map>
#include "cIGZMessageServer.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZMessageServer = 0x64063387;

class cGZMessageServer : public cIGZMessageServer, public cRZSystemService
{
public:
	cGZMessageServer();
	virtual ~cGZMessageServer();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Init();
	virtual bool Shutdown();

public:
	virtual bool MessageSend(cGZMessage const& msg);
	virtual bool MessagePost(cGZMessage const& msg, bool pushFront);

	virtual bool AddNotification(cIGZMessageTarget* target, GZGUID msgType);
	virtual bool RemoveNotification(cIGZMessageTarget* target, GZGUID msgType);

	virtual void GeneralMessagePostToTarget(cGZMessage const& msg, cIGZMessageTarget* target);
	virtual void CancelGeneralMessagePostsToTarget(cIGZMessageTarget* target);

	virtual bool OnTick(uint32_t tickCount);

	virtual int32_t GetMessageQueueSize();

	virtual void SetAlwaysClearQueueOnTick(bool clearOnTick);

protected:
	void DoDelayedNotificationAdditionsAndRemovals();
	void DoDelayedNotificationAdditionsAndRemovalsImmediately();
	void SendGeneralMessages();

protected:
	typedef std::list<cIGZMessageTarget*> MessageTargetList;
	typedef std::map<GZGUID, MessageTargetList> MessageTypeToTargetMap;

	// TODO
};