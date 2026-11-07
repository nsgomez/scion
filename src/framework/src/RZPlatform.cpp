/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "cIGZString.h"
#include "RZPlatform.h"
#include "RZStringUtil.h"

typedef DWORD (WINAPI *pfnGetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);

bool RZIsKeyDownNow(uint32_t key)
{
	if (key == VK_LBUTTON && GetSystemMetrics(SM_SWAPBUTTON) != 0)
	{
		key = VK_RBUTTON;
	}

	return (GetAsyncKeyState(key) >> 15) != 0;
}

int32_t RZThreadSafeAdd(int32_t volatile* addend, int32_t value)
{
	return InterlockedExchangeAdd(reinterpret_cast<LONG volatile*>(addend), value) + value;
}

uint32_t RZGetCurrentAppPath(cIGZString& out)
{
	static bool sTriedWideFunction;
	static pfnGetModuleFileNameW sGetModuleFileNameW;

	if (!sTriedWideFunction)
	{
		HMODULE module = GetModuleHandle("Kernel32.dll");
		if (module != NULL)
		{
			sGetModuleFileNameW = reinterpret_cast<pfnGetModuleFileNameW>(GetProcAddress(module, "GetModuleFileNameW"));
		}

		sTriedWideFunction = true;
	}

	if (sGetModuleFileNameW != NULL)
	{
		wchar_t widePath[MAX_PATH + 1];
		DWORD pathLen = sGetModuleFileNameW(NULL, widePath, MAX_PATH);

		if (pathLen != 0)
		{
			ConvertStringEncoding(widePath, pathLen, GZ_CODEPAGE_UTF16, out);
		}
	}

	if (out.Strlen() == 0)
	{
		char path[MAX_PATH + 1];
		DWORD pathLen = GetModuleFileName(NULL, path, MAX_PATH);

		if (pathLen != 0)
		{
			ConvertStringEncoding(path, pathLen, GZ_CODEPAGE_SYSTEM, out);
		}
	}

	return out.Strlen();
}

bool RZLoadLibraryA(void** handleOut, char const* path)
{
	*handleOut = LoadLibraryA(path);
	return true;
}

bool RZGetProcAddress(void** fnOut, void** module, char const* name)
{
	*fnOut = GetProcAddress(reinterpret_cast<HMODULE>(*module), name);
	return true;
}