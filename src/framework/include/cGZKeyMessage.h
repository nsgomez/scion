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
#include "cIGZKeyMessage.h"

class cGZKeyMessage : public cIGZKeyMessage
{
public:
	cGZKeyMessage();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Create(GZIID iid, void** outPtr) const;

	virtual GZGUID GetType() const;
	virtual void SetType(GZGUID type);

	virtual bool operator== (cIGZMessage2 const& other) const;
	virtual bool operator<  (cIGZMessage2 const& other) const;

public:
	virtual void Initialize(KeyEvent event, uint32_t key, uint32_t keyFlags, uint32_t repeatCount);
	virtual KeyEvent EventType() const;
	virtual uint32_t Key() const;
	virtual uint32_t KeyFlags() const;
	virtual uint32_t RepeatCount() const;

protected:
	uint32_t refCount;
	uint32_t type;
	KeyEvent event;
	uint32_t key;
	uint32_t keyFlags;
	uint32_t repeatCount;
};