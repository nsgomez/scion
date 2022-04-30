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

#include "cGZCOM.h"
#include "cGZFramework.h"
#include "cRZCOMDllDirector.h"
#include "RZStatics.h"

static const GZGUID RZSRVID_cGZCOM = 0xA3CD8DB3;

cGZCOM::cGZCOM() :
	cRZSystemService(RZSRVID_cGZCOM, 10000000)
{
}

cGZCOM::~cGZCOM()
{
}

bool cGZCOM::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZCOM)
	{
		*outPtr = static_cast<cIGZCOM*>(this);
		AddRef();
		return true;
	}
	
	return cRZSystemService::QueryInterface(iid, outPtr);
}

uint32_t cGZCOM::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZCOM::Release()
{
	return cRZSystemService::Release();
}

bool cGZCOM::GetClassObject(GZCLSID clsid, GZIID iid, void** outPtr)
{
	criticalSection.Lock();

	tIdMap::iterator it = clsidMap.find(clsid);
	if (it == clsidMap.end())
	{
		criticalSection.Unlock();

		if (clsid == GZCLSID_cGZFramework)
		{
			return framework->QueryInterface(iid, outPtr);
		}

		return comDirector->GetClassObject(clsid, iid, outPtr);
	}

	cGZCOMLibrary* const comLibrary = (*it).second.second;
	criticalSection.Unlock();

	return GetLibObject(*comLibrary, clsid, iid, outPtr);
}

cIGZFramework* cGZCOM::Framework()
{
	return framework;
}

bool cGZCOM::AddLibrary(const cIGZString& path)
{
	cGZCOMLibrary lib(path);

	criticalSection.Lock();
	tLibraries::iterator it(libraries.find(lib));

	if (it != libraries.end())
	{
		criticalSection.Unlock();
		return true;
	}

	it = libraries.insert(lib).first;
	criticalSection.Unlock();

	cGZCOMLibrary& insertedLib = const_cast<cGZCOMLibrary&>(*it);
	if (UpdateClassRegistry(insertedLib))
	{
		cIGZCOMDirector* const libDirector = insertedLib.GetDirector();
		if (libDirector)
		{
			libDirector->OnStart(this);
		}

		return true;
	}

	return false;
}

void cGZCOM::FreeUnusedLibraries(void)
{
	criticalSection.Lock();

	for (tLibraries::iterator it = libraries.begin(); it != libraries.end(); ++it)
	{
		cGZCOMLibrary& library = const_cast<cGZCOMLibrary&>(*it);
		if (library.IsLoaded() && CanUnloadLibrary(library))
		{
			library.Free();
		}
	}

	criticalSection.Unlock();
}

bool cGZCOM::CreateGuid(GZGUID* outGuid)
{
	// TODO
	return false;
}

bool cGZCOM::CreateGuids(uint32_t guids[], uint32_t count)
{
	// TODO
	return false;
}

bool cGZCOM::RealInit()
{
	if (!initialized)
	{
		framework = RZGetFramework();
		comDirector = RZGetCOMDllDirector();

		cRZString appPath;
		// RZGetCurrentAppPath(appPath); // TODO

		if (comDirector->InitializeCOM(this, appPath))
		{
			initialized = true;
			return true;
		}

		return false;
	}

	return true;
}

void cGZCOM::RealShutdown()
{
	if (initialized)
	{
		FreeAllLibraries();
		framework = NULL;
		initialized = false;
	}
}

void cGZCOM::SetServiceRunning(bool running)
{
	cRZSystemService::SetServiceRunning(true);
}

uint32_t cGZCOM::EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count)
{
	uint32_t actualLibraryCount(this->libraries.size());

	if (libraries != NULL)
	{
		tLibraries::iterator it(this->libraries.begin());
		uint32_t i = 0;

		if (count > actualLibraryCount)
		{
			count = actualLibraryCount;
		}

		while (i < count)
		{
			cGZCOMLibrary* const currentLib = const_cast<cGZCOMLibrary*>(&(*it));
			libraries[i] = currentLib;
			libraries[i]->AddRef();

			++i;
			++it;
		}
	}
	else
	{
		count = actualLibraryCount;
	}

	return count;
}

void cGZCOM::EnumerateLibraries(GZCOMLibraryIterationFunction iterator, void* context)
{
	if (iterator != NULL)
	{
		tLibraries::iterator it(libraries.begin());
		tLibraries::iterator itEnd(libraries.end());

		while (it != itEnd)
		{
			cGZCOMLibrary* const currentLib = const_cast<cGZCOMLibrary*>(&(*it));
			if (!iterator(currentLib, context))
			{
				break;
			}

			++it;
		}
	}
}

bool cGZCOM::Init()
{
	return true;
}

bool cGZCOM::Shutdown()
{
	return true;
}

void cGZCOM::AddEntryCallback(GZCLSID clsid, uint32_t version, void* context)
{
	cGZCOM* const com = reinterpret_cast<cGZCOM*>(context);

	tIdMap::iterator it(com->clsidMap.find(clsid));
	if ((it != com->clsidMap.end()) && ((*it).second.first < version))
	{
		com->clsidMap.erase(it);
	}

	com->clsidMap.insert(std::pair<const GZCLSID, tClassObjectData>(clsid, tClassObjectData(version, com->currentLibrary)));
}

bool cGZCOM::CanUnloadLibrary(cGZCOMLibrary& lib)
{
	if (lib.IsLoaded())
	{
		cIGZCOMDirector* const libDirector = lib.GetDirector();
		return libDirector->CanUnloadNow();
	}

	return true;
}

void cGZCOM::FreeAllLibraries()
{
	criticalSection.Lock();

	for (tLibraries::iterator it = libraries.begin(); it != libraries.end(); ++it)
	{
		cGZCOMLibrary& library = const_cast<cGZCOMLibrary&>(*it);
		if (library.IsLoaded())
		{
			library.Free();
		}
	}

	criticalSection.Unlock();
}

bool cGZCOM::GetLibObject(cGZCOMLibrary& lib, GZCLSID clsid, GZIID iid, void** outPtr)
{
	if (!lib.IsLoaded() && !lib.Load())
	{
		return false;
	}

	cIGZCOMDirector* const libDirector = lib.GetDirector();
	return libDirector->GetClassObject(clsid, iid, outPtr);
}

bool cGZCOM::UpdateClassRegistry(cGZCOMLibrary& lib)
{
	if (lib.Load())
	{
		cIGZCOMDirector* const libDirector = lib.GetDirector();

		criticalSection.Lock();
		currentLibrary = &lib;
		libDirector->EnumClassObjects(cGZCOM::AddEntryCallback, static_cast<cGZCOM*>(this));
		criticalSection.Unlock();
	}

	return false;
}