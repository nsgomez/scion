#include "cGZFramework.h"

cGZFramework::cGZFramework()
{
}

cGZFramework::~cGZFramework()
{
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
		isServicePriorityMapLocked = 1;

		tServicesPriorityMap::iterator prioMapIterator;
		for (prioMapIterator = servicesByPriority.begin(); prioMapIterator != servicesByPriority.end(); ++prioMapIterator)
		{
			if ((*prioMapIterator).second == service)
			{
				servicesByPriority.erase(prioMapIterator);
				break;
			}
		}

		isServicePriorityMapLocked = 0;

		/////////////////////////////////////
		// Remove from servicesById
		/////////////////////////////////////
		isServiceIdMapLocked = 1;

		tServicesIdMap::iterator idMapIterator;
		for (idMapIterator = servicesById.begin(); idMapIterator != servicesById.end(); ++idMapIterator)
		{
			if ((*idMapIterator).second == service)
			{
				servicesById.erase(idMapIterator);
				break;
			}
		}

		isServiceIdMapLocked = 0;
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
}

bool cGZFramework::EnumSystemServices(cIGZUnknownEnumerator* enumerator, cIGZUnknown* unknown1, uint32_t unknown2)
{
}

bool cGZFramework::AddHook(cIGZFrameworkHooks* hook)
{
}

bool cGZFramework::RemoveHook(cIGZFrameworkHooks* hook)
{
}

bool cGZFramework::AddToTick(cIGZSystemService* service)
{
}

bool cGZFramework::RemoveFromTick(cIGZSystemService* service)
{
}

bool cGZFramework::AddToOnIdle(cIGZSystemService* service)
{
}

bool cGZFramework::RemoveFromOnIdle(cIGZSystemService* service)
{
}

int32_t cGZFramework::GetOnIdleInterval(void) const
{
	return this->onIdleInterval;
}

void cGZFramework::SetOnIdleInterval(int32_t intervalInMs)
{
	int32_t actualInterval = 1;
	if (intervalInMs > 0)
	{
		actualInterval = intervalInMs;
	}

	this->onIdleInterval = actualInterval;
}

void cGZFramework::OnTick(uint32_t unknown)
{
}

void cGZFramework::OnIdle(void)
{
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
	this->hasQuit = true;
	this->exitCode = exitCode;
}

void cGZFramework::AbortiveQuit(int exitCode)
{
	Quit(exitCode);
	::exit(this->exitCode);
}

cRZCmdLine* cGZFramework::CommandLine(void)
{
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
}

cIGZOStream* cGZFramework::StdErr(void)
{
}

cIGZIStream* cGZFramework::StdIn(void)
{
}

bool cGZFramework::GetStream(int32_t streamNum, GZREFIID iid, void** outPtr)
{
}

bool cGZFramework::SetStream(int32_t streamNum, cIGZUnknown* stream)
{
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

void cGZFramework::ReportException(char* exceptionReport)
{
}

cRZExceptionNotification* cGZFramework::ExceptionNotificationObj(void) const
{
}

bool cGZFramework::PreAppInit(void)
{
}

bool cGZFramework::AppInit(void)
{
}

bool cGZFramework::PostAppInit(void)
{
}

bool cGZFramework::PreAppShutdown(void)
{
}

bool cGZFramework::AppShutdown(void)
{
}

bool cGZFramework::PostAppShutdown(void)
{
}

cIGZFramework* cGZFramework::AsIGZFramework(void)
{
	return static_cast<cIGZFramework*>(this);
}

void cGZFramework::Run(void)
{
}

void* cGZFramework::GetWindowsInstance(void)
{
}

HWND cGZFramework::GetMainHWND(void)
{
}

void cGZFramework::SetMainHWND(HWND hwnd)
{
}

bool cGZFramework::sInit(cRZCmdLine* cmdLine, bool unknown1)
{
}

bool cGZFramework::sRun(void)
{
}

void cGZFramework::sSetApplication(cIGZApp* app)
{
}

void cGZFramework::sSetFramework(cGZFramework* framework)
{
}