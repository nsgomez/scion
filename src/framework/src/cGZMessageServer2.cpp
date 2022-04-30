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

#include "cGZMessageServer2.h"
#include "cIGZMessage2.h"
#include "cIGZMessageTarget2.h"

cGZMessageServer2::cGZMessageServer2() :
	cRZSystemService(RZSRVID_cGZMessageServer2, 5000000)
{
}

cGZMessageServer2::~cGZMessageServer2()
{
}

bool cGZMessageServer2::QueryInterface(GZREFIID iid, void** outPtr)
{
}

uint32_t cGZMessageServer2::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZMessageServer2::Release()
{
	return cRZSystemService::Release();
}

bool cGZMessageServer2::Init()
{
}

bool cGZMessageServer2::Shutdown()
{
}

bool cGZMessageServer2::MessageSend(cIGZMessage2* msg)
{
}

bool cGZMessageServer2::MessagePost(cIGZMessage2* msg, bool pushFront)
{
}

bool cGZMessageServer2::AddNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
}

bool cGZMessageServer2::RemoveNotification(cIGZMessageTarget2* target, GZGUID msgType)
{
}

void cGZMessageServer2::GeneralMessagePostToTarget(cIGZMessage2* msg, cIGZMessageTarget2* target)
{
}

void cGZMessageServer2::CancelGeneralMessagePostsToTarget(cIGZMessageTarget2* target)
{
}

bool cGZMessageServer2::OnTick(uint32_t tickCount)
{
}

int32_t cGZMessageServer2::GetMessageQueueSize()
{
}

void cGZMessageServer2::SetAlwaysClearQueueOnTick(bool clearOnTick)
{
}

uint32_t cGZMessageServer2::GetRefCount() const
{
}

bool cGZMessageServer2::CreateMessage(GZCLSID clsid, GZREFIID iid, void** msgOut)
{
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovals()
{
}

void cGZMessageServer2::DoDelayedNotificationAdditionsAndRemovalsImmediately()
{
}

void cGZMessageServer2::SendGeneralMessages()
{
}