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
#include <vector>
#include <Windows.h>
#include "cGZCOMLibrary.h"
#include "cIGZCOMDirector.h"
#include "RZPlatform.h"
#include "RZStatics.h"
#include "RZStringUtil.h"

typedef cIGZCOMDirector* (*GZCOMDirectorFactory)();

static bool GZCOMLibrary(HINSTANCE& mHandle, const char* path)
{
	__try
	{
		mHandle = LoadLibrary(path);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

static bool GZCOMLibraryW(HINSTANCE& mHandle, const wchar_t* path)
{
	__try
	{
		mHandle = LoadLibraryW(path);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

static void GZCOMFreeLibrary(HINSTANCE mHandle, const char* path)
{
	__try
	{
		FreeLibrary(mHandle);
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

cGZCOMLibrary::cGZCOMLibrary(const cIGZString& libraryPath) :
	mLoaded(false),
	mRefCount(0),
	mDirector(NULL),
	mLibraryPath(libraryPath),
	mHandle(NULL)
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
	return ++mRefCount;
}

uint32_t cGZCOMLibrary::Release()
{
	return --mRefCount;
}

bool cGZCOMLibrary::Load()
{
	if (!mLoaded)
	{
		cRZString nativePath;
		ConvertStringEncoding(mLibraryPath, nativePath, GZ_CODEPAGE_SYSTEM);

		if (GetVersion() & 0x80000000)
		{
			GZCOMLibrary(mHandle, nativePath.ToChar());
		}
		else
		{
			uint32_t length = mLibraryPath.Strlen();
			std::vector<wchar_t> buffer(length + 1);
			ConvertStringEncoding(mLibraryPath, &buffer[0], length, GZ_CODEPAGE_UTF16);
			buffer[length] = 0;

			GZCOMLibraryW(mHandle, &buffer[0]);
		}

		if (mHandle == NULL)
		{
			cRZString message;
			LPVOID systemMessage = NULL;

			FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&systemMessage),
				0,
				NULL);

			if (systemMessage != NULL)
			{
				Sprintf(message, "Failed to load the requested dll \"%s\"\nWindows error message - \"%s\"", nativePath.ToChar(), systemMessage);
				LocalFree(systemMessage);
			}

			return false;
		}
		else
		{
			GZCOMDirectorFactory getDirector = NULL;
			mLoaded = true;

			HINSTANCE module = mHandle;
			RZGetProcAddress(reinterpret_cast<void**>(&getDirector), reinterpret_cast<void**>(&module), "GZDllGetGZCOMDirector");

			if (getDirector == NULL)
			{
				Free();
				return false;
			}

			cIGZCOMDirector* newDirector = getDirector();
			if (newDirector == NULL)
			{
				cRZString message;
				Sprintf(message, "cGZCOMLibrary::Load: Failed to acquire GZCOM director from library: \"%s\"\n", mLibraryPath.ToChar());
				Free();
				return false;
			}

			mDirector = newDirector;
			if (!newDirector->InitializeCOM(GZCOM(), mLibraryPath))
			{
				cRZString message;
				Sprintf(message, "cGZCOMLibrary::Load: GZCOM Director failed initialization in library: \"%s\"\n", mLibraryPath.ToChar());
				Free();
				return false;
			}
		}
	}

	return true;
}

bool cGZCOMLibrary::Free()
{
	if (mLoaded)
	{
		if (mDirector)
		{
			mDirector = NULL;
		}

		GZCOMFreeLibrary(mHandle, mLibraryPath.ToChar());
		mHandle = NULL;
		mLoaded = false;
	}

	return true;
}

void cGZCOMLibrary::GetPath(cIGZString& path) const
{
	path.Copy(this->mLibraryPath);
}

bool cGZCOMLibrary::SetPath(const cIGZString& path)
{
	if (!mLoaded)
	{
		this->mLibraryPath.Copy(path);
		return true;
	}

	return false;
}

HINSTANCE cGZCOMLibrary::GetHandle() const
{
	return mHandle;
}

void cGZCOMLibrary::SetHandle(HINSTANCE mHandle)
{
	this->mHandle = mHandle;
}

cIGZCOMDirector* cGZCOMLibrary::GetDirector() const
{
	return mDirector;
}

void cGZCOMLibrary::SetDirector(cIGZCOMDirector* mDirector)
{
	if (this->mDirector != mDirector)
	{
		if (mDirector)
		{
			mDirector->AddRef();
		}

		if (this->mDirector)
		{
			this->mDirector->Release();
		}

		this->mDirector = mDirector;
	}
}

bool cGZCOMLibrary::IsLoaded() const
{
	return mLoaded;
}

void cGZCOMLibrary::SetLoaded(bool mLoaded)
{
	this->mLoaded = mLoaded;
}

bool cGZCOMLibrary::operator< (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return mLibraryPath < comparand;
}

bool cGZCOMLibrary::operator> (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return mLibraryPath > comparand;
}

bool cGZCOMLibrary::operator== (const cGZCOMLibrary& other) const
{
	cRZString comparand;
	other.GetPath(comparand);
	return mLibraryPath == comparand;
}