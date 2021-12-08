#include "cGZFramework.h"
#include "cIGZApp.h"

cIGZApp* cGZFramework::mpApp = NULL;
cGZFramework* cGZFramework::mpFramework = NULL;
cGZFramework* cGZFramework::mpSavedFramework = NULL;
int cGZFramework::mnReturnCode = 0;

cGZFramework::cGZFramework()
{
	// TODO
	// exceptionNotificationObj = new cRZExceptionNotification();
}

cGZFramework::~cGZFramework()
{
	// TODO
	/*if (exceptionNotificationObj)
	{
		exceptionNotificationObj->Release();
	}

	if (stdIn)
	{
		stdIn->Release();
	}

	if (stdErr)
	{
		stdErr->Release();
	}

	if (stdOut)
	{
		stdOut->Release();
	}*/
}

bool cGZFramework::QueryInterface(GZREFIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZFramework:
		*outPtr = static_cast<cIGZFramework*>(this);
		break;

	case GZIID_cIGZFrameworkW32:
		*outPtr = static_cast<cIGZFrameworkW32*>(this);
		break;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(static_cast<cIGZFramework*>(this));
		break;

	default:
		return false;
	}

	AddRef();
	return true;
}

uint32_t cGZFramework::AddRef()
{
	frameworkMutex.Lock();
	int oldRefCount = this->refCount;
	this->refCount = oldRefCount + 1;
	frameworkMutex.Unlock();

	return oldRefCount + 1;
}

uint32_t cGZFramework::Release()
{
	frameworkMutex.Lock();

	int newRefCount = 0;
	if (this->refCount > 1)
	{
		newRefCount = this->refCount - 1;
		this->refCount = newRefCount;
	}

	frameworkMutex.Unlock();
	return newRefCount;
}

bool cGZFramework::AddSystemService(cIGZSystemService* service)
{
	if (!isServiceIdMapLocked && !isServicePriorityMapLocked)
	{
		if (service == NULL)
		{
			return false;
		}

		frameworkMutex.Lock();
		
		int32_t priority = service->GetServicePriority();
		GZGUID serviceId = service->GetServiceID();

		tServicesIdMap::iterator it = servicesById.find(serviceId);
		if (it == servicesById.end())
		{
			servicesByPriority.insert(tServicesPriorityMap::value_type(priority, cRZAutoRefCount<cIGZSystemService>(service)));
			
			servicesById.resize(servicesById.size() + 1);
			servicesById.insert(tServicesIdMap::value_type(serviceId, cRZAutoRefCount<cIGZSystemService>(service)));

			frameworkMutex.Unlock();
			return true;
		}
		
		frameworkMutex.Unlock();
	}

	return false;
}

bool cGZFramework::RemoveSystemService(cIGZSystemService* service)
{
	bool result;
	frameworkMutex.Lock();

	if (!isServiceIdMapLocked && !isServicePriorityMapLocked)
	{
		/////////////////////////////////////
		// Remove from activeIdleListeners
		/////////////////////////////////////
		tServicesList::iterator idleListenersBegin = activeIdleListeners.begin();
		if (service != NULL)
		{
			service->AddRef();
		}

		tServicesList::iterator idleListenersEnd = activeIdleListeners.end();
		tServicesList::iterator idleListenersIterator = idleListenersBegin;

		while (idleListenersIterator != idleListenersEnd)
		{
			if ((*idleListenersIterator) == service)
			{
				break;
			}

			++idleListenersIterator;
		}

		if (service != NULL)
		{
			service->Release();
		}

		if (idleListenersIterator != idleListenersEnd)
		{
			activeIdleListeners.erase(idleListenersIterator);
		}

		/////////////////////////////////////
		// Remove from activeTickListeners
		/////////////////////////////////////
		tServicesList::iterator tickListenersBegin = activeTickListeners.begin();
		if (service != NULL)
		{
			service->AddRef();
		}

		tServicesList::iterator tickListenersEnd = activeTickListeners.end();
		tServicesList::iterator tickListenersIterator = tickListenersBegin;

		while (tickListenersIterator != tickListenersEnd)
		{
			if ((*tickListenersIterator) == service)
			{
				break;
			}

			++tickListenersIterator;
		}

		if (service != NULL)
		{
			service->Release();
		}

		if (tickListenersIterator != tickListenersEnd)
		{
			activeTickListeners.erase(tickListenersIterator);
		}

		/////////////////////////////////////
		// Remove from servicesByPriority
		/////////////////////////////////////
		isServicePriorityMapLocked = true;

		for (tServicesPriorityMap::iterator it = servicesByPriority.begin(); it != servicesByPriority.end(); ++it)
		{
			if (it->second == service)
			{
				servicesByPriority.erase(it);
				break;
			}
		}

		isServicePriorityMapLocked = false;

		/////////////////////////////////////
		// Remove from servicesById
		/////////////////////////////////////
		isServiceIdMapLocked = true;

		for (tServicesIdMap::iterator it = servicesById.begin(); it != servicesById.end(); ++it)
		{
			if (it->second == service)
			{
				servicesById.erase(it);
				break;
			}
		}

		isServiceIdMapLocked = false;
		result = true;
	}
	else
	{
		result = false;
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::GetSystemService(GZGUID serviceId, GZREFIID iid, void** outPtr)
{
	bool result;

	frameworkMutex.Lock();
	tServicesIdMap::iterator it = servicesById.find(serviceId);
	isServiceIdMapLocked = true;

	if (it == servicesById.end())
	{
		result = false;
	}
	else
	{
		result = it->second->QueryInterface(iid, outPtr);
	}

	isServiceIdMapLocked = false;
	frameworkMutex.Unlock();

	return result;
}

bool cGZFramework::EnumSystemServices(cIGZUnknownEnumerator* enumerator, cIGZUnknown* unknown1, uint32_t unknown2)
{
	// TODO
	return false;
}

bool cGZFramework::AddHook(cIGZFrameworkHooks* hook)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isHookListLocked)
	{
		tHooksList::iterator it = hooks.begin();
		tHooksList::iterator itEnd = hooks.end();

		while (it != itEnd)
		{
			if ((*it) == hook)
			{
				frameworkMutex.Unlock();
				return false;
			}

			++it;
		}

		hooks.push_back(cRZAutoRefCount<cIGZFrameworkHooks>(hook));
		result = true;
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::RemoveHook(cIGZFrameworkHooks* hook)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isHookListLocked)
	{
		tHooksList::iterator it = hooks.begin();
		tHooksList::iterator itEnd = hooks.end();

		while (it != itEnd)
		{
			if ((*it) == hook)
			{
				hooks.erase(it);
				result = true;
				break;
			}

			++it;
		}
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::AddToTick(cIGZSystemService* service)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isOnTickListLocked)
	{
		int tickPriority = service->GetServiceTickPriority();
		int serviceId = service->GetServiceID();

		tServicesList::iterator it = activeTickListeners.begin();
		tServicesList::iterator itEnd = activeTickListeners.end();
		int itTickPriority;

		do
		{
			if (it == itEnd)
			{
				break;
			}

			cIGZSystemService* itSvc = (*it);
			if (itSvc->GetServiceID() == service->GetServiceID())
			{
				frameworkMutex.Unlock();
				return false;
			}

			itTickPriority = itSvc->GetServiceTickPriority();
			++it;
		}
		while (tickPriority <= itTickPriority);

		activeTickListeners.insert(it, cRZAutoRefCount<cIGZSystemService>(service));
		result = true;
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::RemoveFromTick(cIGZSystemService* service)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isOnTickListLocked)
	{
		if (pendingTickFrames > 0)
		{
			removedTickListeners.push_back(cRZAutoRefCount<cIGZSystemService>(service));
		}

		tServicesList::iterator it = activeTickListeners.begin();
		tServicesList::iterator itEnd = activeTickListeners.end();

		while (it != itEnd)
		{
			if ((*it) == service)
			{
				activeTickListeners.erase(it);
				result = true;
				break;
			}

			++it;
		}
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::AddToOnIdle(cIGZSystemService* service)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isOnIdleListLocked)
	{
		tServicesList::iterator it = activeIdleListeners.begin();
		tServicesList::iterator itEnd = activeIdleListeners.end();

		while (it != itEnd)
		{
			if ((*it) == service)
			{
				frameworkMutex.Unlock();
				return false;
			}

			++it;
		}

		activeIdleListeners.push_back(cRZAutoRefCount<cIGZSystemService>(service));
		result = true;
	}

	frameworkMutex.Unlock();
	return result;
}

bool cGZFramework::RemoveFromOnIdle(cIGZSystemService* service)
{
	frameworkMutex.Lock();
	bool result = false;

	if (!isOnIdleListLocked)
	{
		if (pendingIdleFrames > 0)
		{
			removedIdleListeners.push_back(cRZAutoRefCount<cIGZSystemService>(service));
		}

		tServicesList::iterator it = activeIdleListeners.begin();
		tServicesList::iterator itEnd = activeIdleListeners.end();

		while (it != itEnd)
		{
			if ((*it) == service)
			{
				activeIdleListeners.erase(it);
				result = true;
				break;
			}

			++it;
		}
	}

	frameworkMutex.Unlock();
	return result;
}

int32_t cGZFramework::GetOnIdleInterval(void) const
{
	return this->onIdleInterval;
}

void cGZFramework::SetOnIdleInterval(int32_t idleFrameInterval)
{
	int32_t actualInterval = 1;
	if (idleFrameInterval > 0)
	{
		actualInterval = idleFrameInterval;
	}

	this->onIdleInterval = idleFrameInterval;
}

void cGZFramework::OnTick(int32_t totalTickFrames)
{
	frameworkMutex.Lock();

	++pendingTickFrames;
	std::vector<cIGZSystemService*> tickServiceVec;
	isOnTickListLocked = true;

	tServicesList::iterator copyIt = activeTickListeners.begin();
	tServicesList::iterator copyItEnd = activeTickListeners.end();

	while (copyIt != copyItEnd)
	{
		tickServiceVec.push_back(*copyIt);
		++copyIt;
	}

	isOnTickListLocked = false;
	frameworkMutex.Unlock();

	for (std::vector<cIGZSystemService*>::iterator it = tickServiceVec.begin(); it != tickServiceVec.end(); ++it)
	{
		(*it)->OnTick(totalTickFrames);
	}

	if (pendingTickFrames == 1)
	{
		removedTickListeners.clear();
	}

	--pendingTickFrames;
}

void cGZFramework::OnIdle()
{
	this->OnTick(totalTickFrames++);

	frameworkMutex.Lock();
	++pendingIdleFrames;

	if (totalTickFrames % onIdleInterval == 0)
	{
		int savedIdleFrames = totalIdleFrames;
		++totalIdleFrames;

		std::vector<cIGZSystemService*> idleServiceVec;
		isOnIdleListLocked = true;

		tServicesList::iterator copyIt = activeIdleListeners.begin();
		tServicesList::iterator copyItEnd = activeIdleListeners.end();

		while (copyIt != copyItEnd)
		{
			idleServiceVec.push_back(*copyIt);
			++copyIt;
		}

		isOnIdleListLocked = false;
		frameworkMutex.Unlock();

		for (std::vector<cIGZSystemService*>::iterator it = idleServiceVec.begin(); it != idleServiceVec.end(); ++it)
		{
			(*it)->OnIdle(savedIdleFrames);
		}
	}
	else
	{
		frameworkMutex.Unlock();
	}

	if (pendingIdleFrames == 1)
	{
		removedTickListeners.clear();
	}

	--pendingIdleFrames;
}

bool cGZFramework::IsTickEnabled(void) const
{
	return this->ticksEnabled;
}

void cGZFramework::ToggleTick(bool toggle)
{
	this->ticksEnabled = toggle;
}

void cGZFramework::Quit(int exitCode)
{
	if (false)
	{
		PostQuitMessage(exitCode);
	}
	else
	{
		this->hasQuit = true;
		this->exitCode = exitCode;
	}
}

void cGZFramework::AbortiveQuit(int exitCode)
{
	Quit(exitCode);
	::exit(this->exitCode);
}

cRZCmdLine* cGZFramework::CommandLine(void)
{
	// TODO
	return NULL;
}

bool cGZFramework::IsInstall(void) const
{
	return this->isInstall;
}

cIGZCOM* cGZFramework::GetCOMObject(void)
{
	return static_cast<cIGZCOM*>(&this->com);
}

FrameworkState cGZFramework::GetState(void) const
{
	return this->state;
}

cIGZDebugStream* cGZFramework::GetDebugStream(void)
{
	return this->debugStream;
}

cIGZDebugStream* cGZFramework::DefaultDebugStream(void)
{
	return GetDebugStream();
}

cIGZDebugStream* cGZFramework::DebugStream(void)
{
	return GetDebugStream();
}

void cGZFramework::SetDebugStream(cIGZDebugStream* debugStream)
{
	this->debugStream = debugStream;
}

void cGZFramework::SetDebugLevel(int32_t level)
{
}

int cGZFramework::GetDebugLevel(void) const
{
	return this->debugLevel;
}

cIGZOStream* cGZFramework::StdOut(void)
{
	// TODO
	return NULL;
}

cIGZOStream* cGZFramework::StdErr(void)
{
	// TODO
	return NULL;
}

cIGZIStream* cGZFramework::StdIn(void)
{
	// TODO
	return NULL;
}

bool cGZFramework::GetStream(int32_t streamNum, GZREFIID iid, void** outPtr)
{
	// TODO
	return false;
}

bool cGZFramework::SetStream(int32_t streamNum, cIGZUnknown* stream)
{
	// TODO
	return false;
}

void cGZFramework::SetApplication(cIGZApp* app)
{
	frameworkMutex.Lock();
	cGZFramework::mpApp = app;
	frameworkMutex.Unlock();
}

cIGZApp* cGZFramework::Application(void) const
{
	return cGZFramework::mpApp;
}

void cGZFramework::ReportException(char const* exceptionReport)
{
	// TODO
}

cRZExceptionNotification* cGZFramework::ExceptionNotificationObj(void) const
{
	// TODO
	return NULL;
}

bool cGZFramework::PreAppInit(void)
{
	cIGZSystemService* appAsSvc;
	cIGZApp* app = Application();

	app->QueryInterface(GZIID_cIGZSystemService, reinterpret_cast<void**>(&appAsSvc));
	AddSystemService(appAsSvc);

	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices);

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		if (service->GetServicePriority() <= -2000000)
		{
			break;
		}

		if (service->Init())
		{
			service->SetServiceRunning(true);
		}
	}

	copiedServices.clear();
	if (appAsSvc)
	{
		appAsSvc->Release();
	}

	return true;
}

bool cGZFramework::AppInit(void)
{
	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices);

	bool result = true;

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		int32_t servicePriority = service->GetServicePriority();

		if (servicePriority <= -2000000)
		{
			if (servicePriority <= -3000000)
			{
				break;
			}

			if (!service->Init())
			{
				cIGZApp* serviceAsApp;
				if (service->QueryInterface(GZIID_cIGZApp, reinterpret_cast<void**>(&serviceAsApp)))
				{
					serviceAsApp->Release();
					result = false;
					break;
				}
			}
			else
			{
				service->SetServiceRunning(true);
			}
		}
	}

	copiedServices.clear();
	return result;
}

bool cGZFramework::PostAppInit(void)
{
	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices);

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		if (service->GetServicePriority() <= -3000000 && service->Init())
		{
			service->SetServiceRunning(true);
		}
	}

	copiedServices.clear();
	return true;
}

bool cGZFramework::PreAppShutdown(void)
{
	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices, true);

	bool result = true;

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		if (service->GetServicePriority() > -3000000)
		{
			break;
		}

		if (!service->Shutdown())
		{
			result = false;
		}
		else
		{
			service->SetServiceRunning(false);
		}
	}

	copiedServices.clear();
	return result;
}

bool cGZFramework::AppShutdown(void)
{
	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices, true);

	bool result = true;

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		int servicePriority = service->GetServicePriority();

		if (servicePriority > -3000000)
		{
			if (servicePriority > -2000000)
			{
				break;
			}

			if (!service->Shutdown())
			{
				result = false;
			}
			else
			{
				service->SetServiceRunning(false);
			}
		}
	}

	copiedServices.clear();
	return result;
}

bool cGZFramework::PostAppShutdown(void)
{
	tServicesList copiedServices;
	MakeSystemServiceListCopy(copiedServices, true);

	bool result = true;

	for (tServicesList::iterator it = copiedServices.begin(); it != copiedServices.end(); ++it)
	{
		cIGZSystemService* service = *it;
		if (service->GetServicePriority() > -2000000)
		{
			if (!service->Shutdown())
			{
				result = false;
			}
			else
			{
				service->SetServiceRunning(false);
			}
		}
	}

	copiedServices.clear();
	return result;
}

cIGZFramework* cGZFramework::AsIGZFramework(void)
{
	return static_cast<cIGZFramework*>(this);
}

void cGZFramework::Run(void)
{
	MSG msg;

	do
	{
		if (!this->ticksEnabled)
		{
			WaitMessage();
		}

		while (PeekMessage(&msg, NULL, 0, 0, 1))
		{
			if (msg.message == WM_QUIT)
			{
				cGZFramework::mnReturnCode = msg.wParam;
				return;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (this->ticksEnabled)
		{
			this->OnIdle();
		}
	}
	while (!hasQuit); // TODO: wrong condition

	cGZFramework::mnReturnCode = this->exitCode;
}

void* cGZFramework::GetWindowsInstance(void)
{
	// TODO
	return NULL;
}

HWND cGZFramework::GetMainHWND(void)
{
	return mainHwnd;
}

void cGZFramework::SetMainHWND(HWND hwnd)
{
	mainHwnd = hwnd;
}

bool cGZFramework::HookPreFrameworkInit()
{
	if (!isHookListLocked)
	{
		tHooksList copiedHooks;
		MakeHookListCopy(copiedHooks);

		for (tHooksList::iterator it = copiedHooks.begin(); it != copiedHooks.end(); ++it)
		{
			cIGZFrameworkHooks* hook = *it;
			hook->PreFrameworkInit();
		}

		copiedHooks.clear();
		return true;
	}
	else
	{
		return false;
	}
}

bool cGZFramework::HookPreAppInit()
{
	if (!isHookListLocked)
	{
		tHooksList copiedHooks;
		MakeHookListCopy(copiedHooks);

		for (tHooksList::iterator it = copiedHooks.begin(); it != copiedHooks.end(); ++it)
		{
			cIGZFrameworkHooks* hook = *it;
			hook->PreAppInit();
		}

		copiedHooks.clear();
		return true;
	}
	else
	{
		return false;
	}
}

bool cGZFramework::HookPostAppInit()
{
	if (!isHookListLocked)
	{
		tHooksList copiedHooks;
		MakeHookListCopy(copiedHooks);

		for (tHooksList::iterator it = copiedHooks.begin(); it != copiedHooks.end(); ++it)
		{
			cIGZFrameworkHooks* hook = *it;
			hook->PostAppInit();
		}

		copiedHooks.clear();
		return true;
	}
	else
	{
		return false;
	}
}

void cGZFramework::MakeHookListCopy(tHooksList& dest)
{
	frameworkMutex.Lock();
	isHookListLocked = true;

	for (tHooksList::iterator it = hooks.begin(); it != hooks.end(); ++it)
	{
		dest.push_back(*it);
	}

	isHookListLocked = false;
	frameworkMutex.Unlock();
}

void cGZFramework::MakeSystemServiceListCopy(tServicesList& dest, bool reversePriority)
{
	frameworkMutex.Lock();
	isServicePriorityMapLocked = true;

	if (!reversePriority)
	{
		for (tServicesPriorityMap::reverse_iterator it = servicesByPriority.rbegin(); it != servicesByPriority.rend(); ++it)
		{
			dest.push_back(it->second);
		}
	}
	else
	{
		for (tServicesPriorityMap::iterator it = servicesByPriority.begin(); it != servicesByPriority.end(); ++it)
		{
			dest.push_back(it->second);
		}
	}

	isServicePriorityMapLocked = false;
	frameworkMutex.Unlock();
}

int cGZFramework::Main(cRZCmdLine const& cmdLine, bool unknown)
{
	if (sInit(cmdLine, unknown))
	{
		sRun();
	}

	cGZFramework::mnReturnCode = 0;
	return 0;
}

bool cGZFramework::sInit(cRZCmdLine const& cmdLine, bool unknown)
{
	// TODO: SetCommandLine
	// TODO: FixCommandLine

	cIGZCOM* com = mpFramework->GetCOMObject();
	if (com->RealInit())
	{
		com->SetServiceRunning(true);
	}

	cIGZSystemService* comAsService;
	if (com->QueryInterface(GZIID_cIGZSystemService, reinterpret_cast<void**>(&comAsService)))
	{
		mpFramework->AddSystemService(comAsService);
		comAsService->Release();
		comAsService = NULL;
	}

	mpApp->AddCOMDirectorsHere();
	mpApp->AddDynamicLibrariesHere();
	mpApp->LoadRegistry();

	mpFramework->state = GZFrameworkState_PreFrameworkInit;

	if (!mpApp->PreFrameworkInit())
	{
		com->RealShutdown();
	}
	else
	{
		mpFramework->HookPreFrameworkInit();

		mpFramework->state = GZFrameworkState_UnknownState2;
		mpFramework->state = GZFrameworkState_PreAppInit;

		mpFramework->PreAppInit();
		mpFramework->HookPreAppInit();

		mpFramework->state = GZFrameworkState_InitializingApp;
		if (mpFramework->AppInit())
		{
			mpFramework->ToggleTick(true);
			mpFramework->state = GZFrameworkState_PostAppInit;

			mpFramework->PostAppInit();
			mpFramework->state = GZFrameworkState_PostFrameworkInit;

			mpApp->PostFrameworkInit();
			mpFramework->HookPostAppInit();

			return mnReturnCode == 0;
		}
	}

	cGZFramework::mnReturnCode = -1;
	return false;
}

bool cGZFramework::sRun(void)
{
	cIGZApp* app = mpFramework->Application();
	mpFramework->state = GZFrameworkState_Running;

	if (!app->GZRun())
	{
		mpFramework->Run();
	}

	return mnReturnCode == 0;
}

void cGZFramework::sSetApplication(cIGZApp* app)
{
	cGZFramework::mpApp = app;
	app->AddRef();
}

void cGZFramework::sSetFramework(cGZFramework* framework)
{
	cGZFramework::mpSavedFramework = framework;
	cGZFramework::mpFramework = framework;

	framework->AddRef();
	mpSavedFramework->AddRef();
}