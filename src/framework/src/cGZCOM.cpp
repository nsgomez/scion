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

#include <ctime>
#include "cGZCOM.h"
#include "cGZFramework.h"
#include "cRZCOMDllDirector.h"
#include "cRZDate.h"
#include "cRZThread.h"
#include "cRZTime.h"
#include "RZPlatform.h"
#include "RZStatics.h"

static const GZGUID RZSRVID_cGZCOM = 0xA3CD8DB3;

cGZCOM::cGZCOM() :
	cRZSystemService(RZSRVID_cGZCOM, 10000000),
	mCriticalSectionPtr(&mCriticalSection),
	mRng(0xFFFFFFFF),
	mInitialized(false),
	mUnused(0),
	mCurrentLibrary(NULL),
	mFramework(NULL),
	mComDirector(NULL),
	mNextGuidGenTime(0)
{
}

cGZCOM::~cGZCOM()
{
}

bool cGZCOM::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid != GZIID_cIGZCOM)
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
	
	*outPtr = static_cast<cIGZCOM*>(this);
	AddRef();
	return true;
}

uint32_t cGZCOM::AddRef()
{
	return cRZSystemService::AddRef();
}

uint32_t cGZCOM::Release()
{
	return RemoveRef();
}

bool cGZCOM::GetClassObject(GZCLSID clsid, GZIID iid, void** outPtr)
{
	mCriticalSection.Lock();

	tIdMap::iterator it = mClsidMap.find(clsid);
	if (it == mClsidMap.end())
	{
		mCriticalSection.Unlock();

		if (clsid == GZCLSID_cGZFramework)
		{
			return mFramework->QueryInterface(iid, outPtr);
		}

		return mComDirector->GetClassObject(clsid, iid, outPtr);
	}

	cGZCOMLibrary* const mComLibrary = (*it).second.second;
	mCriticalSection.Unlock();

	return GetLibObject(*mComLibrary, clsid, iid, outPtr);
}

cIGZFramework* cGZCOM::Framework()
{
	return mFramework;
}

bool cGZCOM::AddLibrary(const cIGZString& path)
{
	cGZCOMLibrary lib(path);

	mCriticalSection.Lock();
	tLibraries::iterator it(mLibraries.find(lib));

	if (it != mLibraries.end())
	{
		mCriticalSection.Unlock();
		return true;
	}

	it = mLibraries.insert(lib).first;
	mCriticalSection.Unlock();

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
	mCriticalSection.Lock();

	for (tLibraries::iterator it = mLibraries.begin(); it != mLibraries.end(); ++it)
	{
		cGZCOMLibrary& library = const_cast<cGZCOMLibrary&>(*it);
		if (library.IsLoaded() && CanUnloadLibrary(library))
		{
			library.Free();
		}
	}

	mCriticalSection.Unlock();
}

static int32_t GetGuidGenSeconds(void)
{
	cRZDate referenceDate(2, 2, 1997);
	cRZDate nowDate;
	cRZTime nowTime;

	uint32_t days = nowDate.DateCode();
	days -= referenceDate.DateCode();

	uint32_t hours = days * 24 + nowTime.Hour();
	uint32_t minutes = hours * 60;
	minutes += nowTime.Minute();

	return (int32_t)(minutes * 60 + nowTime.Timestamp() % 60);
}

bool cGZCOM::CreateGuid(GZGUID* outGuid)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	while (time(NULL) < mNextGuidGenTime)
	{
		cRZThread::SleepCurrentThread(250000);
	}

	uint32_t rand = mRng.RandomUint32Uniform(8) << 29;;
	*outGuid = rand | (GetGuidGenSeconds() & 0x1FFFFFFF);
	
	mNextGuidGenTime = time(NULL) + 1;
	return true;
}

bool cGZCOM::CreateGuids(uint32_t guids[], uint32_t count)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	if (count != 0)
	{
		while (time(NULL) < mNextGuidGenTime)
		{
			cRZThread::SleepCurrentThread(250000);
		}

		int32_t guidGenTime = GetGuidGenSeconds();
		uint32_t prefix = 0;

		for (uint32_t i = 0; i < count; i++)
		{
			guids[i] = (((i / 8) + guidGenTime) & 0x1FFFFFFF) | (i << 29);
		}

		mNextGuidGenTime = time(NULL) + ((count + 7) / 8);
	}

	return true;
}

bool cGZCOM::RealInit()
{
	if (!mInitialized)
	{
		mFramework = RZGetFramework();
		mComDirector = RZGetCOMDllDirector();

		cRZString appPath;
		RZGetCurrentAppPath(appPath);

		if (mComDirector->InitializeCOM(this, appPath))
		{
			mInitialized = true;
			return true;
		}

		return false;
	}

	return true;
}

void cGZCOM::RealShutdown()
{
	if (mInitialized)
	{
		FreeAllLibraries();
		mFramework = NULL;
		mInitialized = false;
	}
}

void cGZCOM::SetServiceRunning(bool running)
{
	cRZSystemService::SetServiceRunning(running);
}

uint32_t cGZCOM::EnumerateLibraries(cIGZCOMLibrary* mLibraries[], uint32_t& count)
{
	uint32_t actualLibraryCount(this->mLibraries.size());

	if (mLibraries != NULL)
	{
		tLibraries::iterator it(this->mLibraries.begin());
		uint32_t i = 0;

		if (count > actualLibraryCount)
		{
			count = actualLibraryCount;
		}

		while (i < count)
		{
			cGZCOMLibrary* const currentLib = const_cast<cGZCOMLibrary*>(&(*it));
			mLibraries[i] = currentLib;
			mLibraries[i]->AddRef();

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
		tLibraries::iterator it(mLibraries.begin());
		tLibraries::iterator itEnd(mLibraries.end());

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

	tIdMap::iterator it(com->mClsidMap.find(clsid));
	if ((it != com->mClsidMap.end()) && ((*it).second.first < version))
	{
		com->mClsidMap.erase(it);
	}

	com->mClsidMap.insert(std::pair<const GZCLSID, tClassObjectData>(clsid, tClassObjectData(version, com->mCurrentLibrary)));
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
	mCriticalSection.Lock();

	for (tLibraries::iterator it = mLibraries.begin(); it != mLibraries.end(); ++it)
	{
		cGZCOMLibrary& library = const_cast<cGZCOMLibrary&>(*it);
		if (library.IsLoaded())
		{
			library.Free();
		}
	}

	mCriticalSection.Unlock();
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

		mCriticalSection.Lock();
		mCurrentLibrary = &lib;
		libDirector->EnumClassObjects(cGZCOM::AddEntryCallback, this);
		mCriticalSection.Unlock();

		return true;
	}

	return false;
}