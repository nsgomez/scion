/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include <cassert>
#include "cIGZCOM.h"
#include "cIGZString.h"
#include "cRZCOMDllDirector.h"

cRZCOMDllDirector::cRZCOMDllDirector()
{
}

cRZCOMDllDirector::~cRZCOMDllDirector()
{
}

bool cRZCOMDllDirector::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZCOMDirector:
		*outPtr = static_cast<cIGZCOMDirector*>(this);
		AddRef();
		return true;

	case GZIID_cIGZFrameworkHooks:
		*outPtr = static_cast<cIGZFrameworkHooks*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(static_cast<cIGZCOMDirector*>(this));
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZCOMDllDirector::AddRef()
{
	return ++refCount;
}

uint32_t cRZCOMDllDirector::Release()
{
	return RemoveRef();
}

bool cRZCOMDllDirector::InitializeCOM(cIGZCOM* com, const cIGZString& libraryPath)
{
	if (com != NULL)
	{
		this->com = com;
		this->framework = com->Framework();
		this->libraryPath = libraryPath;

		for (ChildDirectorArray::iterator it = childDirectors.begin(); it != childDirectors.end(); ++it)
		{
			cRZCOMDllDirector* const director = *it;
			director->InitializeCOM(com, libraryPath);
		}
	}

	return false;
}

bool cRZCOMDllDirector::OnStart(cIGZCOM* pCOM)
{
	return true;
}

void cRZCOMDllDirector::EnumClassObjects(ClassObjectEnumerationCallback callback, void* context)
{
	for (ChildDirectorArray::iterator it = childDirectors.begin(); it != childDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const director = *it;
		director->EnumClassObjects(callback, context);
	}

	for (ClassObjectMap::iterator it2 = classObjectMap.begin(); it2 != classObjectMap.end(); ++it2)
	{
		uint32_t clsid = (*it2).first;
		callback(clsid, 0, context);
	}
}

bool cRZCOMDllDirector::GetClassObject(GZGUID clsid, GZIID iid, void** outPtr)
{
	for (ChildDirectorArray::iterator it = childDirectors.begin(); it != childDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const director = *it;
		if (director->GetClassObject(clsid, iid, outPtr))
		{
			return true;
		}
	}

	ClassObjectMap::iterator it2 = classObjectMap.find(clsid);
	if (it2 != classObjectMap.end())
	{
		FactoryFuncRecord& ffr = (*it2).second;
		if (ffr.second == kFactoryFunctionType1)
		{
			FactoryFunctionPtr1 const ffp1 = reinterpret_cast<FactoryFunctionPtr1>(ffr.first);
			cIGZUnknown* const newObj = ffp1();

			if (newObj->QueryInterface(iid, outPtr))
			{
				return true;
			}

			newObj->Release();
		}
		else if (ffr.second == kFactoryFunctionType2)
		{
			FactoryFunctionPtr2 ffp2 = reinterpret_cast<FactoryFunctionPtr2>(ffr.first);
			return ffp2(iid, outPtr);
		}
		else
		{
			assert(false);
		}
	}

	return false;
}

bool cRZCOMDllDirector::CanUnloadNow(void)
{
	if (refCount == 0)
	{
		for (ChildDirectorArray::iterator it = childDirectors.begin(); it != childDirectors.end(); ++it)
		{
			cRZCOMDllDirector* const director = *it;
			if (!director->CanUnloadNow())
			{
				return false;
			}
		}
	}

	return true;
}

bool cRZCOMDllDirector::OnUnload(void)
{
	return true;
}

uint32_t cRZCOMDllDirector::RefCount(void)
{
	return refCount;
}

uint32_t cRZCOMDllDirector::RemoveRef(void)
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

cIGZFramework* cRZCOMDllDirector::FrameWork(void)
{
	return framework;
}

cIGZCOM* cRZCOMDllDirector::GZCOM(void)
{
	return com;
}

bool cRZCOMDllDirector::GetLibraryPath(cIGZString& path)
{
	path = this->libraryPath;
	return true;
}

uint32_t cRZCOMDllDirector::GetHeapAllocatedSize(void)
{
	return 0;
}

void cRZCOMDllDirector::AddCls(GZCLSID clsid, FactoryFunctionPtr1 ffp1)
{
	DummyFunctionPtr ffp = reinterpret_cast<DummyFunctionPtr>(ffp1);
	FactoryFuncRecord ffr(ffp, kFactoryFunctionType1);

	classObjectMap.insert(std::pair<GZCLSID, FactoryFuncRecord>(clsid, ffr));
}

void cRZCOMDllDirector::AddCls(GZCLSID clsid, FactoryFunctionPtr2 ffp2)
{
	DummyFunctionPtr ffp = reinterpret_cast<DummyFunctionPtr>(ffp2);
	FactoryFuncRecord ffr(ffp, kFactoryFunctionType2);

	classObjectMap.insert(std::pair<GZCLSID, FactoryFuncRecord>(clsid, ffr));
}

void cRZCOMDllDirector::AddDirector(cRZCOMDllDirector* director)
{
	director->InitializeCOM(GZCOM(), libraryPath);

	for (ChildDirectorArray::iterator it = director->childDirectors.begin(); it != director->childDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const childDirector = *it;
		AddDirector(childDirector);
	}

	childDirectors.push_back(director);
}

bool cRZCOMDllDirector::PreFrameworkInit(void)
{
	return true;
}

bool cRZCOMDllDirector::PreAppInit(void)
{
	return true;
}

bool cRZCOMDllDirector::PostAppInit(void)
{
	return true;
}

bool cRZCOMDllDirector::PreAppShutdown(void)
{
	return true;
}

bool cRZCOMDllDirector::PostAppShutdown(void)
{
	return true;
}

bool cRZCOMDllDirector::PostSystemServiceShutdown(void)
{
	return true;
}

bool cRZCOMDllDirector::AbortiveQuit(void)
{
	return true;
}

bool cRZCOMDllDirector::OnInstall(void)
{
	return true;
}