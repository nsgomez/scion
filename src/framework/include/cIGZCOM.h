#pragma once
#include "cIGZUnknown.h"

class cGZCOMLibrary;
class cIGZCOMLibrary;
class cIGZFrameWork;
class cIGZString;
class cIGZSystemService;

class cIGZCOM : public cIGZUnknown
{
public:
	virtual bool GetClassObject(GZGUID clsid, GZREFIID iid, void** outPtr) = 0;

	virtual cIGZFrameWork* FrameWork() = 0;

	virtual bool AddLibrary(const cIGZString& path) = 0;
	virtual void FreeUnusedLibraries(void) = 0;

	virtual bool CreateGuid(GZGUID* outGuid) = 0;
	virtual bool CreateGuids(uint32_t guids[], uint32_t count) = 0;

	virtual bool RealInit() = 0;
	virtual void RealShutdown() = 0;

	virtual cIGZSystemService* SetServiceRunning(bool running) = 0;

	typedef bool(*GZCOMLibraryIterationFunction)(cGZCOMLibrary*, void*);

	virtual uint32_t EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count) = 0;
	virtual void EnumerateLibraries(GZCOMLibaryIterationFunction iterator, void* context) = 0;
};