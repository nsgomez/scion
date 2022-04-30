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

#include "cGZMouseMessage.h"

cGZMouseMessage::cGZMouseMessage() :
	refCount(0),
	type(MSGID_MouseMessage),
	event(MouseEvent::Unknown),
	position(),
	wheelDelta(0),
	keyFlags(0)
{
}

bool cGZMouseMessage::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMouseMessage)
	{
		*outPtr = static_cast<cIGZMouseMessage*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZMessage2)
	{
		*outPtr = static_cast<cIGZMessage2*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZMouseMessage::AddRef()
{
	return ++refCount;
}

uint32_t cGZMouseMessage::Release()
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

bool cGZMouseMessage::Create(GZIID iid, void** outPtr) const
{
	cGZMouseMessage* newMsg = new cGZMouseMessage();

	if (iid == GZIID_cIGZMouseMessage || iid == GZIID_cIGZMessage2 || iid == GZIID_cIGZUnknown)
	{
		*outPtr = newMsg;
		newMsg->refCount = 1;
		return true;
	}
	else
	{
		delete newMsg;
		return false;
	}
}

GZGUID cGZMouseMessage::GetType() const
{
	return type;
}

void cGZMouseMessage::SetType(GZGUID type)
{
	this->type = type;
}

bool cGZMouseMessage::operator== (cIGZMessage2 const& other) const
{
	return this->type == other.GetType();
}

bool cGZMouseMessage::operator< (cIGZMessage2 const& other) const
{
	return this->type < other.GetType();
}

void cGZMouseMessage::Initialize(MouseEvent event, cRZPoint const& position, uint32_t wheelDelta, int32_t keyFlags)
{
	this->event = event;
	this->position = position;
	this->wheelDelta = wheelDelta;
	this->keyFlags = keyFlags;
}

cIGZMouseMessage::MouseEvent cGZMouseMessage::EventType() const
{
	return event;
}

cRZPoint const& cGZMouseMessage::Position() const
{
	return position;
}

uint32_t cGZMouseMessage::WheelDelta() const
{
	return wheelDelta;
}

int32_t cGZMouseMessage::KeyFlags() const
{
	return keyFlags;
}