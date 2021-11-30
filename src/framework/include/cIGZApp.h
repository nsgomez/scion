#pragma once
#include "cIGZUnknown.h"

class cIGZFrameWork;
class cIGZSystemService;

class cIGZApp : public cIGZUnknown
{
public:
	virtual cIGZSystemService* AsIGZSystemService(void) = 0;
	virtual char const* ModuleName(void) = 0;
	virtual cIGZFrameWork FrameWork(void) = 0;

	virtual bool AddApplicationService(cIGZSystemService* service);

	virtual bool PreFrameWorkInit(void) = 0;
	virtual bool PostFrameWorkInit(void) = 0;
	virtual bool PreFrameWorkShutdown(void) = 0;

	virtual bool GZRun(void) = 0;
	
	virtual bool LoadRegistry(void) = 0;

	virtual void AddDynamicLibrariesHere(void) = 0;
	virtual void AddCOMDirectorsHere(void) = 0;
	virtual void AddApplicationServicesHere(void) = 0;
};