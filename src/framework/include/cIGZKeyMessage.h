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
#include "cIGZMessage2.h"

static const GZIID GZIID_cIGZKeyMessage = 0x1A104742;
static const GZGUID MSGID_KeyMessage = 0x7A104750;

class cIGZKeyMessage : public cIGZMessage2
{
public:
	enum KeyEvent // TODO
	{
		Unknown = -1,
	};

public:
	virtual void Initialize(KeyEvent event, uint32_t key, uint32_t keyFlags, uint32_t repeatCount) = 0;
	virtual KeyEvent EventType() const = 0;
	virtual uint32_t Key() const = 0;
	virtual uint32_t KeyFlags() const = 0;
	virtual uint32_t RepeatCount() const = 0;
};