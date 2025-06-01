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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <hash_map>
#include <list>
#include <map>
#include <Windows.h>
#include "cGZCOM.h"
#include "cIGZFramework.h"
#include "cIGZFrameworkHooks.h"
#include "cIGZFrameworkW32.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cRZAutoRefCount.h"
#include "cRZCmdLine.h"
#include "cRZCriticalSection.h"

static const GZGUID GZCLSID_cGZFramework = 0x000003E8;

class cIGZUnknownEnumerator;
class cRZCmdLine;

class cGZFramework : public cIGZFramework, public cIGZFrameworkW32
{
private:
	typedef std::list<cRZAutoRefCount<cIGZFrameworkHooks> > tHooksList;
	typedef std::list<cRZAutoRefCount<cIGZSystemService> > tServicesList;
	typedef std::hash_map<GZGUID, cRZAutoRefCount<cIGZSystemService> > tServicesIdMap;
	typedef std::multimap<int32_t, cRZAutoRefCount<cIGZSystemService> > tServicesPriorityMap;

public:
	cGZFramework();
	virtual ~cGZFramework();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool AddSystemService(cIGZSystemService* service);
	virtual bool RemoveSystemService(cIGZSystemService* service);
	virtual bool GetSystemService(GZGUID serviceId, GZIID iid, void** outPtr);
	virtual bool EnumSystemServices(cIGZUnknownEnumerator* enumerator, cIGZUnknown* context, GZIID iid);

	virtual bool AddHook(cIGZFrameworkHooks* hook);
	virtual bool RemoveHook(cIGZFrameworkHooks* hook);

	virtual bool AddToTick(cIGZSystemService* service);
	virtual bool RemoveFromTick(cIGZSystemService* service);

	virtual bool AddToOnIdle(cIGZSystemService* service);
	virtual bool RemoveFromOnIdle(cIGZSystemService* service);

	virtual int32_t GetOnIdleInterval() const;
	virtual void SetOnIdleInterval(int32_t idleFrameInterval);

	virtual void OnTick(uint32_t totalTickFrames);
	virtual void OnIdle();

	virtual bool IsTickEnabled() const;
	virtual void ToggleTick(bool toggle);

	virtual void Quit(int exitCode);
	virtual void AbortiveQuit(int exitCode);

	virtual cRZCmdLine* CommandLine();

	virtual bool IsInstall() const;

	virtual cIGZCOM* GetCOMObject();
	virtual FrameworkState GetState() const;

	virtual cIGZDebugStream* GetDebugStream();
	virtual cIGZDebugStream* DefaultDebugStream();
	virtual cIGZDebugStream* DebugStream();
	virtual void SetDebugStream(cIGZDebugStream* debugStream);

	virtual void SetDebugLevel(int32_t level);
	virtual int GetDebugLevel() const;

	virtual cIGZOStream* StdOut();
	virtual cIGZOStream* StdErr();
	virtual cIGZIStream* StdIn();

	virtual bool GetStream(int32_t streamNum, GZIID iid, void** outPtr);
	virtual bool SetStream(int32_t streamNum, cIGZUnknown* stream);

	virtual void SetApplication(cIGZApp* app);
	virtual cIGZApp* Application() const;

	virtual void ReportException(char const* exceptionReport);
	virtual cRZExceptionNotification* ExceptionNotificationObj() const;

public:
	virtual bool PreAppInit();
	virtual bool AppInit();
	virtual bool PostAppInit();
	virtual bool PreAppShutdown();
	virtual bool AppShutdown();
	virtual bool PostAppShutdown();

public:
	virtual cIGZFramework* AsIGZFramework();
	virtual void Run();

	virtual HINSTANCE GetWindowsInstance();
	virtual HWND GetMainHWND();
	virtual void SetMainHWND(HWND hwnd);

protected:
	bool HookPreFrameworkInit();
	bool HookPreAppInit();
	bool HookPostAppInit();

	void MakeHookListCopy(tHooksList& dest);
	void MakeSystemServiceListCopy(tServicesList& dest, bool reversePriority = false);

public:
	static int Main(cRZCmdLine const& cmdLine, bool unknown); // TODO

	static bool sInit(cRZCmdLine const& cmdLine, bool unknown); // TODO
	static bool sRun();
	static void sSetApplication(cIGZApp* app);
	static void sSetFramework(cGZFramework* framework);

public:
	static cIGZApp* mpApp;
	static cGZFramework* mpFramework;
	static cGZFramework* mpSavedFramework;
	static int mnReturnCode;

private:
	cGZCOM com;
	uint32_t refCount;

	tHooksList hooks;
	tServicesPriorityMap servicesByPriority;
	tServicesIdMap servicesById;

	tServicesList activeIdleListeners;
	tServicesList removedIdleListeners;
	tServicesList activeTickListeners;
	tServicesList removedTickListeners;

	bool ticksEnabled;
	uint32_t onIdleInterval;
	uint32_t totalTickFrames;
	uint32_t totalIdleFrames;

	bool isInstall;
	FrameworkState state;
	uint8_t isServicePriorityMapLocked;
	uint8_t isServiceIdMapLocked;
	uint8_t isOnIdleListLocked;
	uint8_t isOnTickListLocked;
	uint8_t isHookListLocked;

	uint32_t pendingIdleFrames;
	uint32_t pendingTickFrames;

	cRZCriticalSection criticalSection;
	cRZCmdLine cmdLine;

	cIGZDebugStream* debugStream;
	cRZAutoRefCount<cIGZOStream> stdOut;
	cRZAutoRefCount<cIGZOStream> stdErr;
	cRZAutoRefCount<cIGZIStream> stdIn;
	int debugLevel;

	cRZExceptionNotification* exceptionNotification;

	bool hasQuit;
	int exitCode;

	HINSTANCE windowsInstance;
	HWND mainHwnd;
};