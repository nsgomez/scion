#pragma once
#include "cIGZUnknown.h"

class cIGZFrameworkHooks : public cIGZUnknown
{
public:
	virtual bool PreFrameworkInit(void) = 0;
	virtual bool PreAppInit(void) = 0;
	virtual bool PostAppInit(void) = 0;
	virtual bool PreAppShutdown(void) = 0;
	virtual bool PostAppShutdown(void) = 0;
	virtual bool PostSystemServiceShutdown(void) = 0;
	virtual bool AbortiveQuit(void) = 0;
	virtual bool OnInstall(void) = 0;
};