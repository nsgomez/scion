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
#include <hash_map>
#include <list>
#include <map>
#include <Windows.h>
#include "cGZCOM.h"
#include "cIGZFramework.h"
#include "cIGZFrameworkHooks.h"
#include "cIGZFrameworkW32.h"
#include "cRZAutoRefCount.h"
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
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool AddSystemService(cIGZSystemService* service);
	virtual bool RemoveSystemService(cIGZSystemService* service);
	virtual bool GetSystemService(GZGUID serviceId, GZREFIID iid, void** outPtr);
	virtual bool EnumSystemServices(cIGZUnknownEnumerator* enumerator, cIGZUnknown* unknown1, uint32_t unknown2); // TODO

	virtual bool AddHook(cIGZFrameworkHooks* hook);
	virtual bool RemoveHook(cIGZFrameworkHooks* hook);

	virtual bool AddToTick(cIGZSystemService* service);
	virtual bool RemoveFromTick(cIGZSystemService* service);

	virtual bool AddToOnIdle(cIGZSystemService* service);
	virtual bool RemoveFromOnIdle(cIGZSystemService* service);

	virtual int32_t GetOnIdleInterval() const;
	virtual void SetOnIdleInterval(int32_t idleFrameInterval);

	virtual void OnTick(int32_t totalTickFrames);
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

	virtual bool GetStream(int32_t streamNum, GZREFIID iid, void** outPtr);
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
	virtual void* GetWindowsInstance(); // TODO: unknown return type
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
	uint32_t refCount;                       // 0x88
	tHooksList hooks;                        // 0x90
	tServicesPriorityMap servicesByPriority; // 0x98
	int __unknown_0xA8;
	int __unknown_0xAC;
	tServicesIdMap servicesById;             // 0xB0
	int __unknown_0xC0;
	tServicesList activeIdleListeners;       // 0xC4
	tServicesList removedIdleListeners;      // 0xCC
	tServicesList activeTickListeners;       // 0xD4
	tServicesList removedTickListeners;      // 0xDC
	bool ticksEnabled;                       // 0xE4
	int32_t onIdleInterval;                  // 0xE8
	int32_t totalTickFrames;                 // 0xEC
	int32_t totalIdleFrames;                 // 0xF0
	bool isInstall;                          // 0xF4
	FrameworkState state;                    // 0xF8
	uint8_t isServicePriorityMapLocked;      // 0xFC
	uint8_t isServiceIdMapLocked;            // 0xFD
	uint8_t isOnIdleListLocked;              // 0xFE
	uint8_t isOnTickListLocked;              // 0xFF
	uint8_t isHookListLocked;                // 0x100
	int32_t pendingIdleFrames;               // 0x104
	int32_t pendingTickFrames;               // 0x108
	cRZCriticalSection frameworkMutex;       // 0x10C
	// cRZCmdLine cmdLine;                   // 0x114
	cIGZDebugStream* debugStream;            // 0x144
	cIGZOStream* stdOut;                     // 0x148
	cIGZOStream* stdErr;                     // 0x14C
	cIGZIStream* stdIn;                      // 0x150
	int debugLevel;                          // 0x154
	cRZExceptionNotification* exceptionNotificationObj; // 0x158
	bool hasQuit;                            // 0x15C
	int exitCode;                            // 0x160
	void* unknownWindowsInstance;            // 0x164
	HWND mainHwnd;
};