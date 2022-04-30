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
#include "cRZAutoMsg2Notification.h"
#include "cRZSysServPtr.h"

bool cRZAutoMsg2Notification::AddMessageType(GZGUID type)
{
	std::pair<std::set<GZGUID>::iterator, bool> result = msgTypes.insert(type);
	return result.second;
}

bool cRZAutoMsg2Notification::AddMessageTypes(GZGUID const* typePtr, uint32_t count)
{
	bool insertedAll = true;
	while (count > 0)
	{
		std::pair<std::set<GZGUID>::iterator, bool> result = this->msgTypes.insert(*typePtr);
		if (!result.second)
		{
			insertedAll = false;
		}

		++typePtr;
		--count;
	}

	return insertedAll;
}

void cRZAutoMsg2Notification::AddAllNotifications(cIGZMessageTarget2* target, cIGZMessageServer2* server)
{
	if (server == NULL)
	{
		server = cRZSysServPtr<cIGZMessageServer2, GZIID_cIGZMessageServer2, RZSRVID_cGZMessageServer2>();
	}

	for (std::set<GZGUID>::iterator it = msgTypes.begin(); it != msgTypes.end(); ++it)
	{
		server->AddNotification(target, *it);
	}
}

void cRZAutoMsg2Notification::RemoveAllNotifications(cIGZMessageTarget2* target, cIGZMessageServer2* server)
{
	if (server == NULL)
	{
		server = cRZSysServPtr<cIGZMessageServer2, GZIID_cIGZMessageServer2, RZSRVID_cGZMessageServer2>();
	}

	for (std::set<GZGUID>::iterator it = msgTypes.begin(); it != msgTypes.end(); ++it)
	{
		server->RemoveNotification(target, *it);
	}
}