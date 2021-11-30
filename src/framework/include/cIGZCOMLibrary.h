#pragma once
#include "cIGZUnknown.h"

class cIGZCOMDirector;
class cIGZString;

class cIGZCOMLibrary : public cIGZUnknown
{
public:
	virtual bool GetPath(cIGZString& output) = 0;
	virtual cIGZCOMDirector* GetDirector(void) = 0;

	virtual void* GetHandle(void) = 0;
	virtual bool IsLoaded(void) = 0;
};