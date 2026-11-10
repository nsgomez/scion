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

#pragma once
#include "cGZPersistResourceKey.h"
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZPersistResourceManager = 0x656B8EFC;

class cIGZPersistResourceManager : public cIGZUnknown
{
public:
	virtual bool GetResource(cGZPersistResourceKey const& key, GZIID iid, void** outPtr, uint32_t flags, cIGZUnknown* context) = 0;
	virtual bool GetPrivateResource(cGZPersistResourceKey const& key, GZIID iid, void** outPtr, uint32_t flags, cIGZUnknown* context) = 0;

	virtual bool GetNewResource(cGZPersistResourceKey const& key, GZIID iid, void** outPtr, uint32_t flags, cIGZUnknown* context) = 0;
	virtual bool GetNewResource(uint32_t type, uint32_t group, void** outPtr, uint32_t flags, cIGZUnknown* context) = 0;

	virtual bool RegisterResource(cGZPersistResourceKey const& key, cIGZPersistResource* resource) = 0;
	virtual bool RegisterResource(cGZPersistResourceKey const& key, cIGZPersistResource& resource) = 0;
	virtual bool UnregisterResource(cGZPersistResourceKey const& key) = 0;
	virtual bool HasRegisteredResource(cGZPersistResourceKey const& key) = 0;
};