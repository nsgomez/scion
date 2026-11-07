/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include <Windows.h>
#include "cIGZCOMLibrary.h"
#include "cRZString.h"

class cGZCOMLibrary : public cIGZCOMLibrary
{
public:
	cGZCOMLibrary();
	cGZCOMLibrary(const cIGZString& libraryPath);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual void GetPath(cIGZString& output) const;
	virtual cIGZCOMDirector* GetDirector() const;
	virtual HINSTANCE GetHandle() const;
	virtual bool IsLoaded() const;

	virtual ~cGZCOMLibrary() { }

public:
	bool Load();
	bool Free();

	bool SetPath(const cIGZString& path);
	void SetHandle(HINSTANCE handle);
	void SetDirector(cIGZCOMDirector* director);
	void SetLoaded(bool loaded);

public:
	bool operator<  (const cGZCOMLibrary& other) const;
	bool operator>  (const cGZCOMLibrary& other) const;
	bool operator== (const cGZCOMLibrary& other) const;

protected:
	bool mLoaded;
	uint32_t mRefCount;
	cIGZCOMDirector* mDirector;
	cRZString mLibraryPath;
	HINSTANCE mHandle;
};