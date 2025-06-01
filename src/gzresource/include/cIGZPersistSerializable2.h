/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include "cGZPersistTypeInfo.h"
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistSerializable2 = 0x5B0AAA74;

class cIGZPersistReader;
class cIGZPersistWriter;

class cIGZPersistSerializable2 : public cIGZUnknown
{
public:
	virtual void ReadFrom(cIGZPersistReader* reader) = 0;
	virtual cGZPersistTypeInfo TypeInfo(void) const = 0;
	virtual void WriteTo(cIGZPersistWriter* writer) const = 0;
};