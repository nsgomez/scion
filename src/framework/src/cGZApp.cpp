#include "cGZApp.h"
#include "cIGZFramework.h"
#include "RZStatics.h"

cGZApp::cGZApp(char const* moduleName) :
	cRZSystemService(RZSRVID_cGZApp, -2000000),
	moduleName(moduleName)
{
}

cGZApp::~cGZApp()
{
}

bool cGZApp::QueryInterface(GZREFIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZApp)
	{
		*outPtr = static_cast<cIGZApp*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZApp::AddRef(void)
{
	return cRZSystemService::AddRef();
}

uint32_t cGZApp::Release(void)
{
	return cRZSystemService::Release();
}

cIGZSystemService* cGZApp::AsIGZSystemService(void)
{
	return this;
}

char const* cGZApp::ModuleName(void)
{
	return moduleName.c_str();
}

cIGZFramework* cGZApp::Framework(void)
{
	return RZGetFramework();
}

bool cGZApp::AddApplicationService(cIGZSystemService* service)
{
	return Framework()->AddSystemService(service);
}

bool cGZApp::PreFrameworkInit(void)
{
	return true;
}

bool cGZApp::PostFrameworkInit(void)
{
	return true;
}

bool cGZApp::PreFrameworkShutdown(void)
{
	return true;
}

bool cGZApp::GZRun(void)
{
	return false;
}

bool cGZApp::LoadRegistry(void)
{
	return true;
}

void cGZApp::AddDynamicLibrariesHere(void)
{
}

void cGZApp::AddCOMDirectorsHere(void)
{
}

void cGZApp::AddApplicationServicesHere(void)
{
}