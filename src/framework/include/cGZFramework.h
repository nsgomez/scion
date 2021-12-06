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

class cGZFramework : public cIGZFramework, public cIGZFrameworkW32
{
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
	virtual void SetOnIdleInterval(int32_t intervalInMs);

	virtual void OnTick(uint32_t unknown); // TODO
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

	virtual void ReportException(char* exceptionReport);
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

public:
	static bool sInit(cRZCmdLine* cmdLine, bool unknown1); // TODO
	static bool sRun();
	static void sSetApplication(cIGZApp* app);
	static void sSetFramework(cGZFramework* framework);

public:
	static cIGZApp* mpApp;
	static cGZFramework* mpFramework;
	static cGZFramework* mpSavedFramework;
	static int mnReturnCode;

private:
	typedef std::list<cRZAutoRefCount<cIGZFrameworkHooks> > tHooksList;
	typedef std::list<cRZAutoRefCount<cIGZSystemService> > tServicesList;
	typedef std::hash_map<GZGUID, cRZAutoRefCount<cIGZSystemService> > tServicesIdMap;
	typedef std::multimap<int32_t, cRZAutoRefCount<cIGZSystemService> > tServicesPriorityMap;

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
	int32_t onIdleInterval;
	bool isInstall;
	FrameworkState state;
	uint8_t isServicePriorityMapLocked;
	uint8_t isServiceIdMapLocked;
	uint8_t isOnIdleListLocked;
	uint8_t isOnTickListLocked;
	uint8_t isHookListLocked;
	uint32_t pendingIdleFrames;
	uint32_t pendingTickFrames;
	cRZCriticalSection frameworkMutex;
	// cRZCmdLine cmdLine;
	cIGZDebugStream* debugStream;
	cIGZOStream* stdOut;
	cIGZOStream* stdErr;
	cIGZIStream* stdIn;
	int debugLevel;
	cRZExceptionNotification* exceptionNotificationObj;
	bool hasQuit;
	int exitCode;
	void* unknownWindowsInstance;
};