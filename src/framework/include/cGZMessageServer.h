/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <deque>
#include <hash_map>
#include <list>
#include "cGZMessageQueue.h"
#include "cIGZMessageServer.h"
#include "cRZCriticalSection.h"
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
	struct DelayedNotificationInfo
	{
	public:
		DelayedNotificationInfo(bool isAddition, cIGZMessageTarget* target, GZGUID msgType) :
			isAddition(isAddition),
			target(target),
			msgType(msgType)
		{
		}

		bool isAddition;
		cIGZMessageTarget* target;
		GZGUID msgType;
	};

	struct GeneralMessageTargetInfo
	{
	public:
		GeneralMessageTargetInfo(GeneralMessageTargetInfo const& other);
		GeneralMessageTargetInfo(cIGZMessageTarget* target, cGZMessage const& msg) :
			target(target),
			msg(msg)
		{
		}

		cIGZMessageTarget* target;
		cGZMessage msg;
	};

	void DoDelayedNotificationAdditionsAndRemovals();
	void DoDelayedNotificationAdditionsAndRemovalsImmediately();
	void SendGeneralMessages();

protected:
	typedef std::list<cIGZMessageTarget*> MessageTargetList;
	typedef std::list<DelayedNotificationInfo> DelayedNotificationList;
	typedef std::hash_map<GZGUID, MessageTargetList*> MessageTargetMap;
	typedef std::pair<GZGUID, MessageTargetList*> MessageTargetMapEntry;
	typedef std::deque<GeneralMessageTargetInfo> GeneralMessageDeque;

	cRZCriticalSection notificationLock;
	cRZCriticalSection messageQueueLock;
	cRZCriticalSection generalMessagePostLock;

	cGZMessageQueue messageQueue;
	uint64_t totalMessagesDequeued;

	int32_t pendingNotificationOps;
	bool alwaysClearQueueOnTick;

	DelayedNotificationList delayedNotificationChanges;
	MessageTargetMap notificationTargets;

	int32_t pendingGeneralMessageCount;
	GeneralMessageDeque generalMessages;
};