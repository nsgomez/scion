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

#include "cGZMessage.h"
#include "cGZMessageServer.h"
#include "cIGZMessageTarget.h"

cGZMessageServer::cGZMessageServer() :
	cRZSystemService(RZSRVID_cGZMessageServer, 5000000)
{
}

cGZMessageServer::~cGZMessageServer()
{
}

bool cGZMessageServer::QueryInterface(GZIID iid, void** outPtr)
{
}

uint32_t cGZMessageServer::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZMessageServer::Release()
{
	return cRZSystemService::Release();
}

bool cGZMessageServer::Init()
{
}

bool cGZMessageServer::Shutdown()
{
}

bool cGZMessageServer::MessageSend(cGZMessage const& msg)
{
}

bool cGZMessageServer::MessagePost(cGZMessage const& msg, bool pushFront)
{
}

bool cGZMessageServer::AddNotification(cIGZMessageTarget* target, GZGUID msgType)
{
}

bool cGZMessageServer::RemoveNotification(cIGZMessageTarget* target, GZGUID msgType)
{
}

void cGZMessageServer::GeneralMessagePostToTarget(cGZMessage const& msg, cIGZMessageTarget* target)
{
}

void cGZMessageServer::CancelGeneralMessagePostsToTarget(cIGZMessageTarget* target)
{
}

bool cGZMessageServer::OnTick(uint32_t tickCount)
{
}

int32_t cGZMessageServer::GetMessageQueueSize()
{
}

void cGZMessageServer::SetAlwaysClearQueueOnTick(bool clearOnTick)
{
}

void cGZMessageServer::DoDelayedNotificationAdditionsAndRemovals()
{
}

void cGZMessageServer::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
}

void cGZMessageServer::SendGeneralMessages()
{
}