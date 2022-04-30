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
#include <map>
#include <set>
#include "cGZCOMLibrary.h"
#include "cIGZCOM.h"
#include "cIGZString.h"
#include "cRZCriticalSection.h"
#include "cRZRandom.h"
#include "cRZSystemService.h"

class cRZCOMDllDirector;

class cGZCOM : public cIGZCOM, public cRZSystemService
{
public:
	cGZCOM();
	virtual ~cGZCOM();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool GetClassObject(GZCLSID clsid, GZIID iid, void** outPtr);

	virtual cIGZFramework* Framework();

	virtual bool AddLibrary(const cIGZString& path);
	virtual void FreeUnusedLibraries(void);

	virtual bool CreateGuid(GZGUID* outGuid);
	virtual bool CreateGuids(uint32_t guids[], uint32_t count);

	virtual bool RealInit();
	virtual void RealShutdown();

	virtual void SetServiceRunning(bool running);

	virtual uint32_t EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count);
	virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterator, void* context);

public:
	virtual bool Init();
	virtual bool Shutdown();

protected:
	static void AddEntryCallback(GZCLSID clsid, uint32_t version, void* context);

	bool CanUnloadLibrary(cGZCOMLibrary& lib);
	void FreeAllLibraries();
	bool GetLibObject(cGZCOMLibrary& lib, GZCLSID clsid, GZIID iid, void** outPtr);
	bool UpdateClassRegistry(cGZCOMLibrary& lib);

protected:
	typedef std::pair<uint32_t, cGZCOMLibrary*> tClassObjectData;
	typedef std::map<GZCLSID, tClassObjectData> tIdMap;
	typedef std::set<cGZCOMLibrary> tLibraries;

	cRZCriticalSection criticalSection;
	bool initialized;
	uint32_t unused;
	tIdMap clsidMap;
	tLibraries libraries;
	cGZCOMLibrary* currentLibrary;
	cIGZFramework* framework;
	cRZCOMDllDirector* comDirector;
	cRZRandom rng;
	int nextGuidGenTime;
};