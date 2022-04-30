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
#include "cIGZMessageServer2.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZMessageServer2 = 0x04FA845B;

class cGZMessageServer2 : public cIGZMessageServer2, public cRZSystemService
{
public:
	cGZMessageServer2();
	virtual ~cGZMessageServer2();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Init();
	virtual bool Shutdown();

public:
	virtual bool MessageSend(cIGZMessage2* msg);
	virtual bool MessagePost(cIGZMessage2* msg, bool pushFront);

	virtual bool AddNotification(cIGZMessageTarget2* target, GZGUID msgType);
	virtual bool RemoveNotification(cIGZMessageTarget2* target, GZGUID msgType);

	virtual void GeneralMessagePostToTarget(cIGZMessage2* msg, cIGZMessageTarget2* target);
	virtual void CancelGeneralMessagePostsToTarget(cIGZMessageTarget2* target);

	virtual bool OnTick(uint32_t tickCount);

	virtual int32_t GetMessageQueueSize();

	virtual void SetAlwaysClearQueueOnTick(bool clearOnTick);

	virtual uint32_t GetRefCount() const;

	virtual bool CreateMessage(GZCLSID clsid, GZIID iid, void** msgOut);

protected:
	void DoDelayedNotificationAdditionsAndRemovals();
	void DoDelayedNotificationAdditionsAndRemovalsImmediately();
	void SendGeneralMessages();

protected:
	typedef std::list<cIGZMessageTarget2*> MessageTarget2List;
	typedef std::map<GZGUID, MessageTarget2List> MessageTypeToTarget2Map;

	// TODO
};