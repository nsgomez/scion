#pragma once
#include <Windows.h>
#include "cIGZCOMLibrary.h"
#include "cRZString.h"

class cGZCOMLibrary : public cIGZCOMLibrary
{
public:
	cGZCOMLibrary();
	cGZCOMLibrary(const cIGZString& libraryPath);
	virtual ~cGZCOMLibrary();

public:
	virtual bool QueryInterface(GZREFIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Load();
	virtual bool Free();

	virtual void GetPath(cIGZString& output) const;
	virtual bool SetPath(const cIGZString& path);

	virtual HINSTANCE GetHandle() const;
	virtual void SetHandle(HINSTANCE handle);

	virtual cIGZCOMDirector* GetDirector() const;
	virtual void SetDirector(cIGZCOMDirector* director);

	virtual bool IsLoaded() const;
	virtual void SetLoaded(bool loaded);

public:
	bool operator<  (const cGZCOMLibrary& other) const;
	bool operator>  (const cGZCOMLibrary& other) const;
	bool operator== (const cGZCOMLibrary& other) const;

protected:
	bool loaded;
	uint32_t refCount;
	cIGZCOMDirector* director;
	cRZString libraryPath;
	HINSTANCE handle;
};