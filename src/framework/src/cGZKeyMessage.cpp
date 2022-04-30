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

#include "cGZKeyMessage.h"

cGZKeyMessage::cGZKeyMessage() :
	refCount(0),
	type(MSGID_KeyMessage),
	event(KeyEvent::Unknown),
	key(0),
	keyFlags(0),
	repeatCount(0)
{
}

bool cGZKeyMessage::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZKeyMessage)
	{
		*outPtr = static_cast<cIGZKeyMessage*>(this);
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

uint32_t cGZKeyMessage::AddRef()
{
	return ++refCount;
}

uint32_t cGZKeyMessage::Release()
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

bool cGZKeyMessage::Create(GZIID iid, void** outPtr) const
{
	cGZKeyMessage* newMsg = new cGZKeyMessage();

	if (iid == GZIID_cIGZKeyMessage || iid == GZIID_cIGZMessage2 || iid == GZIID_cIGZUnknown)
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

GZGUID cGZKeyMessage::GetType() const
{
	return type;
}

void cGZKeyMessage::SetType(GZGUID type)
{
	this->type = type;
}

bool cGZKeyMessage::operator== (cIGZMessage2 const& other) const
{
	return this->type == other.GetType();
}

bool cGZKeyMessage::operator< (cIGZMessage2 const& other) const
{
	return this->type < other.GetType();
}

void cGZKeyMessage::Initialize(KeyEvent event, uint32_t key, uint32_t keyFlags, uint32_t repeatCount)
{
	this->event = event;
	this->key = key;
	this->keyFlags = keyFlags;
	this->repeatCount = repeatCount;
}

cIGZKeyMessage::KeyEvent cGZKeyMessage::EventType() const
{
	return event;
}

uint32_t cGZKeyMessage::Key() const
{
	return key;
}

uint32_t cGZKeyMessage::KeyFlags() const
{
	return keyFlags;
}

uint32_t cGZKeyMessage::RepeatCount() const
{
	return repeatCount;
}