#pragma once
#include "cIGZUnknown.h"

class cIGZCOM;
class cIGZFrameWork;
class cIGZString;
class cRZCOMDllDirector;

class cIGZCOMDirector : public cIGZUnknown
{
public:
	typedef void(*ClassObjectEnumerationCallback)(GZGUID, uint32_t, void*);

	virtual bool InitializeCOM(cIGZCOM* pCOM, const cIGZString& libraryPath) = 0;
	virtual bool OnStart(cIGZCOM* pCOM) = 0;

	virtual void EnumClassObjects(ClassObjectEnumerationCallback callback, void* context) = 0;
	virtual bool GetClassObject(GZGUID clsid, GZREFIID iid, void** outPtr) = 0;

	virtual bool CanUnloadNow(void) = 0;
	virtual bool OnUnload(void) = 0;

	virtual uint32_t RefCount(void) = 0;
	virtual uint32_t RemoveRef(void) = 0;

	virtual cIGZFrameWork* FrameWork(void) = 0;
	virtual cIGZCOM* GZCOM(void) = 0;

	virtual void AddDirector(cIGZCOMDirector* director) = 0;
	virtual bool GetLibraryPath(cIGZString& path) = 0;
	virtual uint32_t GetHeapAllocatedSize(void) = 0;
};