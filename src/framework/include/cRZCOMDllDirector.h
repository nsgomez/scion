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

#pragma once
#include <map>
#include <vector>
#include "cIGZCOMDirector.h"
#include "cIGZFrameworkHooks.h"
#include "cRZString.h"

class cIGZCOM;

class cRZCOMDllDirector : public cIGZCOMDirector, public cIGZFrameworkHooks
{
public:
	cRZCOMDllDirector();
	virtual ~cRZCOMDllDirector();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	typedef void (*DummyFunctionPtr)();
	typedef cIGZUnknown* (*FactoryFunctionPtr1)();
	typedef bool (*FactoryFunctionPtr2)(uint32_t, void**);

	virtual bool InitializeCOM(cIGZCOM* com, const cIGZString& libraryPath);
	virtual bool OnStart(cIGZCOM* com);

	virtual void EnumClassObjects(ClassObjectEnumerationCallback callback, void* context);
	virtual bool GetClassObject(GZGUID clsid, GZIID iid, void** outPtr);

	virtual bool CanUnloadNow(void);
	virtual bool OnUnload(void);

	virtual uint32_t RefCount(void);
	virtual uint32_t RemoveRef(void);

	virtual cIGZFramework* FrameWork(void);
	virtual cIGZCOM* GZCOM(void);

	virtual GZGUID GetDirectorID() const = 0;
	virtual bool GetLibraryPath(cIGZString& path);
	virtual uint32_t GetHeapAllocatedSize(void);

public:
	void AddCls(GZCLSID clsid, FactoryFunctionPtr1 ffp);
	void AddCls(GZCLSID clsid, FactoryFunctionPtr2 ffp);
	void AddDirector(cRZCOMDllDirector* director);

public:
	virtual bool PreFrameworkInit(void);
	virtual bool PreAppInit(void);
	virtual bool PostAppInit(void);
	virtual bool PreAppShutdown(void);
	virtual bool PostAppShutdown(void);
	virtual bool PostSystemServiceShutdown(void);
	virtual bool AbortiveQuit(void);
	virtual bool OnInstall(void);

protected:
	enum FactoryFunctionType
	{
		kFactoryFunctionType1 = 1,
		kFactoryFunctionType2 = 2,
	};

	typedef std::vector<cRZCOMDllDirector*> ChildDirectorArray;
	typedef std::pair<DummyFunctionPtr, FactoryFunctionType> FactoryFuncRecord;
	typedef std::map<GZCLSID, FactoryFuncRecord> ClassObjectMap;

	uint32_t refCount;
	cRZString libraryPath;
	cIGZFramework* framework;
	cIGZCOM* com;
	ChildDirectorArray childDirectors;
	ClassObjectMap classObjectMap;
};