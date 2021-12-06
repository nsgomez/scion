#pragma once
#include <string>
#include "cIGZApp.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZApp = 0x00000066;

class cGZApp : public cRZSystemService, public cIGZApp
{
public:
	cGZApp(char const* moduleName);
	virtual ~cGZApp();

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual cIGZSystemService* AsIGZSystemService();
	virtual char const* ModuleName();
	virtual cIGZFramework* Framework();

	virtual bool AddApplicationService(cIGZSystemService* service);

	virtual bool PreFrameworkInit();
	virtual bool PostFrameworkInit();
	virtual bool PreFrameworkShutdown();

	virtual bool GZRun();
	
	virtual bool LoadRegistry();

	virtual void AddDynamicLibrariesHere();
	virtual void AddCOMDirectorsHere();
	virtual void AddApplicationServicesHere();

protected:
	std::string moduleName;
};