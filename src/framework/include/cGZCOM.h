#pragma once
#include <map>
#include <set>
#include "cGZCOMLibrary.h"
#include "cIGZCOM.h"
#include "cIGZString.h"
#include "cRZCriticalSection.h"
#include "cRZSystemService.h"

class cGZCOM : public cIGZCOM, public cRZSystemService
{
public:
	cGZCOM();
	virtual ~cGZCOM();

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool GetClassObject(GZREFCLSID clsid, GZREFIID iid, void** outPtr);

	virtual cIGZFramework* Framework();

	virtual bool AddLibrary(const cIGZString& path);
	virtual void FreeUnusedLibraries(void);

	virtual bool CreateGuid(GZGUID* outGuid);
	virtual bool CreateGuids(uint32_t guids[], uint32_t count);

	virtual bool RealInit();
	virtual void RealShutdown();

	virtual void SetServiceRunning(bool running);

	virtual uint32_t EnumerateLibraries(cIGZCOMLibrary* libraries[], uint32_t& count);
	virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterator, void* context);

public:
	virtual bool Init();
	virtual bool Shutdown();

protected:
	static void AddEntryCallback(GZREFCLSID clsid, uint32_t version, void* context);

	bool CanUnloadLibrary(cGZCOMLibrary& lib);
	void FreeAllLibraries();
	bool GetLibObject(cGZCOMLibrary& lib, GZREFCLSID clsid, GZREFIID iid, void** outPtr);
	bool UpdateClassRegistry(cGZCOMLibrary& lib);

protected:
	typedef std::pair<uint32_t, cGZCOMLibrary*> tClassObjectData;
	typedef std::map<GZREFCLSID, tClassObjectData> tIdMap;
	typedef std::set<cGZCOMLibrary> tLibraries;

	cRZCriticalSection criticalSection;
	bool initialized;
	uint32_t unused;
	tIdMap clsidMap;
	tLibraries libraries;
	cGZCOMLibrary* currentLibrary;
	cIGZFramework* framework;
	cIGZCOMDirector* comDirector;
	// RZRandom random; // TODO
	int nextGuidGenTime;
};