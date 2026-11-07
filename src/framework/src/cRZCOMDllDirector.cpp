/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cIGZCOM.h"
#include "cIGZString.h"
#include "cRZCOMDllDirector.h"

cRZCOMDllDirector::cRZCOMDllDirector() :
	mRefCount(0),
	mUnused(0),
	mCom(NULL),
	mFramework(NULL)
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
	return ++mRefCount;
}

uint32_t cRZCOMDllDirector::Release()
{
	return RemoveRef();
}

bool cRZCOMDllDirector::InitializeCOM(cIGZCOM* mCom, const cIGZString& mLibraryPath)
{
	if (mCom != NULL)
	{
		this->mCom = mCom;
		this->mFramework = mCom->Framework();
		this->mLibraryPath = mLibraryPath;

		for (ChildDirectorArray::iterator it = mChildDirectors.begin(); it != mChildDirectors.end(); ++it)
		{
			(*it)->InitializeCOM(mCom, mLibraryPath);
		}

		return true;
	}

	return false;
}

bool cRZCOMDllDirector::OnStart(cIGZCOM* pCOM)
{
	return true;
}

void cRZCOMDllDirector::EnumClassObjects(ClassObjectEnumerationCallback callback, void* context)
{
	for (ChildDirectorArray::iterator it = mChildDirectors.begin(); it != mChildDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const director = *it;
		director->EnumClassObjects(callback, context);
	}

	for (ClassObjectMap::iterator it2 = mClassObjectMap.begin(); it2 != mClassObjectMap.end(); ++it2)
	{
		uint32_t clsid = (*it2).first;
		callback(clsid, 0, context);
	}
}

bool cRZCOMDllDirector::GetClassObject(GZGUID clsid, GZIID iid, void** outPtr)
{
	for (ChildDirectorArray::iterator it = mChildDirectors.begin(); it != mChildDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const director = *it;
		if (director->GetClassObject(clsid, iid, outPtr))
		{
			return true;
		}
	}

	ClassObjectMap::iterator it2 = mClassObjectMap.find(clsid);
	if (it2 != mClassObjectMap.end())
	{
		FactoryFuncRecord& ffr = (*it2).second;
		switch (ffr.second)
		{
		case kFactoryFunctionType2:
			{
				FactoryFunctionPtr2 ffp2 = reinterpret_cast<FactoryFunctionPtr2>(ffr.first);
				return ffp2(iid, outPtr);
			}

		case kFactoryFunctionType1:
			{
				cIGZUnknown* const newObj = reinterpret_cast<FactoryFunctionPtr1>(ffr.first)();
				if (newObj->QueryInterface(iid, outPtr))
				{
					return true;
				}

				newObj->AddRef();
				newObj->Release();
				break;
			}

		default:
			break;
		}
	}

	return false;
}

bool cRZCOMDllDirector::CanUnloadNow(void)
{
	if (mRefCount == 0)
	{
		for (ChildDirectorArray::iterator it = mChildDirectors.begin(); it != mChildDirectors.end(); ++it)
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
	return mRefCount;
}

uint32_t cRZCOMDllDirector::RemoveRef(void)
{
	if (mRefCount > 0)
	{
		--mRefCount;
	}

	return mRefCount;
}

cIGZFramework* cRZCOMDllDirector::FrameWork(void)
{
	return mFramework;
}

cIGZCOM* cRZCOMDllDirector::GZCOM(void)
{
	return mCom;
}

bool cRZCOMDllDirector::GetLibraryPath(cIGZString& path)
{
	path.FromChar(mLibraryPath.ToChar(), mLibraryPath.Strlen());
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

	mClassObjectMap.insert(std::pair<GZCLSID, FactoryFuncRecord>(clsid, ffr));
}

void cRZCOMDllDirector::AddCls(GZCLSID clsid, FactoryFunctionPtr2 ffp2)
{
	DummyFunctionPtr ffp = reinterpret_cast<DummyFunctionPtr>(ffp2);
	FactoryFuncRecord ffr(ffp, kFactoryFunctionType2);

	mClassObjectMap.insert(std::pair<GZCLSID, FactoryFuncRecord>(clsid, ffr));
}

void cRZCOMDllDirector::AddDirector(cRZCOMDllDirector* director)
{
	director->InitializeCOM(GZCOM(), mLibraryPath);

	for (ChildDirectorArray::iterator it = director->mChildDirectors.begin(); it != director->mChildDirectors.end(); ++it)
	{
		cRZCOMDllDirector* const childDirector = *it;
		AddDirector(childDirector);
	}

	mChildDirectors.push_back(director);
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