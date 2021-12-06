#pragma once
#include <Windows.h>
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZFrameworkW32 = 0x23EA70A1;

class cIGZFramework;

class cIGZFrameworkW32 : public cIGZUnknown
{
public:
	virtual cIGZFramework* AsIGZFramework(void) = 0;
	virtual void Run(void) = 0;
	virtual void* GetWindowsInstance(void) = 0; // TODO: unknown return type
	virtual HWND GetMainHWND(void) = 0;
	virtual void SetMainHWND(HWND hwnd) = 0;
};