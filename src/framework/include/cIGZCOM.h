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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZCOM = 0x4241330D;

class cGZCOMLibrary;
class cIGZCOMLibrary;
class cIGZFramework;
class cIGZString;
class cIGZSystemService;

class cIGZCOM : public cIGZUnknown
{
public:
	virtual bool GetClassObject(GZGUID clsid, GZIID iid, void** outPtr) = 0;

	virtual cIGZFramework* Framework() = 0;

	virtual bool AddLibrary(const cIGZString& path) = 0;
	virtual void FreeUnusedLibraries(void) = 0;

	virtual bool CreateGuid(GZGUID* outGuid) = 0;
	virtual bool CreateGuids(uint32_t guids[], uint32_t count) = 0;

	virtual bool RealInit() = 0;
	virtual void RealShutdown() = 0;

	virtual void SetServiceRunning(bool running) = 0;

	typedef bool (*GZCOMLibraryIterationFunction)(cGZCOMLibrary*, void*);

	virtual uint32_t EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count) = 0;
	virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterator, void* context) = 0;
};