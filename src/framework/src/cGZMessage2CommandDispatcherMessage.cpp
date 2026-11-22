/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGZMessage2CommandDispatcherMessage.h"

cGZMessage2CommandDispatcherMessage::cGZMessage2CommandDispatcherMessage()
{
}

cGZMessage2CommandDispatcherMessage::~cGZMessage2CommandDispatcherMessage()
{
}

bool cGZMessage2CommandDispatcherMessage::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZCommandDispatcherObject:
		*outPtr = static_cast<cIGZCommandDispatcherObject*>(this);
		AddRef();
		return true;

	case GZIID_cGZMessage2CommandDispatcherMessage:
		*outPtr = static_cast<cGZMessage2CommandDispatcherMessage*>(this);
		AddRef();
		return true;

	default:
		return cRZMessage2Standard::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZMessage2CommandDispatcherMessage::AddRef()
{
	return cRZMessage2Standard::AddRef();
}

uint32_t cGZMessage2CommandDispatcherMessage::Release()
{
	return cRZMessage2Standard::Release();
}