/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cGZPersistResourceKey.h"
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistResource = 0x456B8F1D;

class cIGZPersistResource : public cIGZUnknown
{
public:
	virtual cGZPersistResourceKey GetKey(void) const = 0;
	virtual void SetKey(cGZPersistResourceKey const& key) = 0;

	virtual uint32_t GetType(void) const = 0;
	virtual uint32_t GetGroup(void) const = 0;
	virtual uint32_t GetInstance(void) const = 0;

	virtual void SetType(uint32_t value) = 0;
	virtual void SetGroup(uint32_t value) = 0;
	virtual void SetInstance(uint32_t value) = 0;

	virtual uint32_t GetRefCount(void) = 0;
};