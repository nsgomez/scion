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
#include "cIGZCOMLibrary.h"
#include "cRZString.h"

class cGZCOMLibrary : public cIGZCOMLibrary
{
public:
	cGZCOMLibrary();
	cGZCOMLibrary(const cIGZString& libraryPath);
	virtual ~cGZCOMLibrary();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Load();
	virtual bool Free();

	virtual void GetPath(cIGZString& output) const;
	virtual bool SetPath(const cIGZString& path);

	virtual HINSTANCE GetHandle() const;
	virtual void SetHandle(HINSTANCE handle);

	virtual cIGZCOMDirector* GetDirector() const;
	virtual void SetDirector(cIGZCOMDirector* director);

	virtual bool IsLoaded() const;
	virtual void SetLoaded(bool loaded);

public:
	bool operator<  (const cGZCOMLibrary& other) const;
	bool operator>  (const cGZCOMLibrary& other) const;
	bool operator== (const cGZCOMLibrary& other) const;

protected:
	bool loaded;
	uint32_t refCount;
	cIGZCOMDirector* director;
	cRZString libraryPath;
	HINSTANCE handle;
};