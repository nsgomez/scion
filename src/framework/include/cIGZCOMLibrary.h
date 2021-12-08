/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include <Windows.h>
#include "cIGZUnknown.h"

class cIGZCOMDirector;
class cIGZString;

static const GZREFIID GZIID_cIGZCOMLibrary = 0x07D4922E;

class cIGZCOMLibrary : public cIGZUnknown
{
public:
	virtual bool Load() = 0;
	virtual bool Free() = 0;

	virtual void GetPath(cIGZString& output) const = 0;
	virtual bool SetPath(const cIGZString& path) = 0;

	virtual HINSTANCE GetHandle() const = 0;
	virtual void SetHandle(HINSTANCE handle) = 0;

	virtual cIGZCOMDirector* GetDirector() const = 0;
	virtual void SetDirector(cIGZCOMDirector* director) = 0;

	virtual bool IsLoaded() const = 0;
	virtual void SetLoaded(bool loaded) = 0;
};