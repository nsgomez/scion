#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZApp = 0x00000066;

class cIGZFramework;
class cIGZSystemService;

class cIGZApp : public cIGZUnknown
{
public:
	virtual cIGZSystemService* AsIGZSystemService() = 0;
	virtual char const* ModuleName() = 0;
	virtual cIGZFramework* Framework() = 0;

	virtual bool AddApplicationService(cIGZSystemService* service) = 0;

	virtual bool PreFrameworkInit() = 0;
	virtual bool PostFrameworkInit() = 0;
	virtual bool PreFrameworkShutdown() = 0;

	virtual bool GZRun() = 0;
	
	virtual bool LoadRegistry() = 0;

	virtual void AddDynamicLibrariesHere() = 0;
	virtual void AddCOMDirectorsHere() = 0;
	virtual void AddApplicationServicesHere() = 0;
};