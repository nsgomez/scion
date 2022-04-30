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
#include "cIGZMouseMessage.h"

class cGZMouseMessage : public cIGZMouseMessage
{
public:
	cGZMouseMessage();

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
	virtual void Initialize(MouseEvent event, cRZPoint const& position, uint32_t wheelDelta, int32_t keyFlags);
	virtual MouseEvent EventType() const;
	virtual cRZPoint const& Position() const;
	virtual uint32_t WheelDelta() const;
	virtual int32_t KeyFlags() const;

protected:
	uint32_t refCount;
	uint32_t type;
	MouseEvent event;
	cRZPoint position;
	uint32_t wheelDelta;
	int32_t keyFlags;
};