/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cGZCOMLibrary.h"
#include "cIGZCOMDirector.h"
#include "RZPlatform.h"
#include "RZStatics.h"

static bool GZCOMLibrary(HINSTANCE& handle, const char* path)
{
	__try
	{
		handle = LoadLibrary(path);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

static bool GZCOMLibraryW(HINSTANCE& handle, const wchar_t* path)
{
	__try
	{
		handle = LoadLibraryW(path);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

static void GZCOMFreeLibrary(HINSTANCE handle, const char* path)
{
	__try
	{
		FreeLibrary(handle);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

static bool GZCOMGetProcAddress(cIGZCOMDirector*(*&func)(), const HINSTANCE& handle, const char* funcName)
{
	__try
	{
		func = (cIGZCOMDirector*(*)())GetProcAddress(handle, funcName);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

cGZCOMLibrary::cGZCOMLibrary()
{
}

cGZCOMLibrary::cGZCOMLibrary(const cIGZString& libraryPath)
	: libraryPath(libraryPath)
{
}

cGZCOMLibrary::~cGZCOMLibrary()
{
}

bool cGZCOMLibrary::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZCOMLibrary)
	{
		*outPtr = static_cast<cIGZCOMLibrary*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZCOMLibrary::AddRef()
{
	return ++refCount;
}

uint32_t cGZCOMLibrary::Release()
{
	return --refCount;
}

bool cGZCOMLibrary::Load()
{
	if (loaded)
	{
		return true;
	}

	// TODO: call ConvertStringEncoding, use Unicode if available
	char const* path = libraryPath.ToChar();
	RZLoadLibraryA(reinterpret_cast<void**>(&handle), path);

	if (handle == NULL)
	{
		LPSTR buffer = NULL;
		va_list args = NULL;

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			GetLastError(),
			LANG_USER_DEFAULT,
			reinterpret_cast<LPSTR>(&buffer),
			0,
			&args);

		cRZString formattedMessage;
		if (buffer != NULL)
		{
			formattedMessage.Sprintf(
				"Failed to load the requested dll \"%s\"\nWindows error message - \"%s\"",
				path,
				buffer);

			LocalFree(reinterpret_cast<HLOCAL>(buffer));
		}
	}
	else
	{
		loaded = true;

		cIGZCOMDirector*(*getDirectorFn)(void) = NULL;
		RZGetProcAddress(reinterpret_cast<void**>(&getDirectorFn), reinterpret_cast<void**>(&handle), "GZDllGetGZCOMDirector");

		if (getDirectorFn != NULL)
		{
			cIGZCOMDirector* director = getDirectorFn();
			if (director == NULL)
			{
				cRZString formattedMessage;
				formattedMessage.Sprintf("cGZCOMLibrary::Load: Failed to acquire GZCOM director from library: \"%s\"\n", libraryPath.ToChar());
			}
			else
			{
				this->director = director;
				if (this->director->InitializeCOM(GZCOM(), cRZString(libraryPath)))
				{
					return true;
				}

				cRZString formattedMessage;
				formattedMessage.Sprintf("cGZCOMLibrary::Load: GZCOM Director failed initialization in library: \"%s\"\n", libraryPath.ToChar());
			}
		}
	}

	return false;
}

bool cGZCOMLibrary::Free()
{
	if (loaded)
	{
		if (director)
		{
			director = NULL;
		}

		GZCOMFreeLibrary(handle, libraryPath.ToChar());
		handle = NULL;
		loaded = false;
	}

	return true;
}

void cGZCOMLibrary::GetPath(cIGZString& path) const
{
	path.Copy(this->libraryPath);
}

bool cGZCOMLibrary::SetPath(const cIGZString& path)
{
	if (!loaded)
	{
		this->libraryPath.Copy(path);
		return true;
	}

	return false;
}

HINSTANCE cGZCOMLibrary::GetHandle() const
{
	return handle;
}

void cGZCOMLibrary::SetHandle(HINSTANCE handle)
{
	this->handle = handle;
}

cIGZCOMDirector* cGZCOMLibrary::GetDirector() const
{
	return director;
}

void cGZCOMLibrary::SetDirector(cIGZCOMDirector* director)
{
	if (this->director != director)
	{
		if (director)
		{
			director->AddRef();
		}

		if (this->director)
		{
			this->director->Release();
		}

		this->director = director;
	}
}

bool cGZCOMLibrary::IsLoaded() const
{
	return loaded;
}

void cGZCOMLibrary::SetLoaded(bool loaded)
{
	this->loaded = loaded;
}

bool cGZCOMLibrary::operator< (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return libraryPath < comparand;
}

bool cGZCOMLibrary::operator> (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return libraryPath > comparand;
}

bool cGZCOMLibrary::operator== (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return libraryPath == comparand;
}