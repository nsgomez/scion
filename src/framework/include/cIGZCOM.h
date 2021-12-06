#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZCOM = 0x4241330D;

class cGZCOMLibrary;
class cIGZCOMLibrary;
class cIGZFramework;
class cIGZString;
class cIGZSystemService;

class cIGZCOM : public cIGZUnknown
{
public:
	virtual bool GetClassObject(GZGUID clsid, GZREFIID iid, void** outPtr) = 0;

	virtual cIGZFramework* Framework() = 0;

	virtual bool AddLibrary(const cIGZString& path) = 0;
	virtual void FreeUnusedLibraries(void) = 0;

	virtual bool CreateGuid(GZGUID* outGuid) = 0;
	virtual bool CreateGuids(uint32_t guids[], uint32_t count) = 0;

	virtual bool RealInit() = 0;
	virtual void RealShutdown() = 0;

	virtual void SetServiceRunning(bool running) = 0;

	typedef bool (*GZCOMLibraryIterationFunction)(cGZCOMLibrary*, void*);

	virtual uint32_t EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count) = 0;
	virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterator, void* context) = 0;
};