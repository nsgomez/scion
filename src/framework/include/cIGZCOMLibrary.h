#pragma once
#include <Windows.h>
#include "cIGZUnknown.h"

class cIGZCOMDirector;
class cIGZString;

static const GZREFIID GZIID_cIGZCOMLibrary = 0x07D4922E;

class cIGZCOMLibrary : public cIGZUnknown
{
public:
	virtual bool Load() = 0;
	virtual bool Free() = 0;

	virtual void GetPath(cIGZString& output) const = 0;
	virtual bool SetPath(const cIGZString& path) = 0;

	virtual HINSTANCE GetHandle() const = 0;
	virtual void SetHandle(HINSTANCE handle) = 0;

	virtual cIGZCOMDirector* GetDirector() const = 0;
	virtual void SetDirector(cIGZCOMDirector* director) = 0;

	virtual bool IsLoaded() const = 0;
	virtual void SetLoaded(bool loaded) = 0;
};