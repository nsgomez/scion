/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cIGZSerializable.h"
#include "cIGZString.h"
#include "cRZMessage2Standard.h"

// TODO: messages have a custom allocator on W32, but just use malloc on POSIX
// Need to figure out how to override `operator new` and `operator delete`

cRZMessage2Standard::cRZMessage2Standard() :
	mRefCount(0),
	mHasType(false),
	mHasData1(false),
	mHasData2(false),
	mHasData3(false),
	mHasData4(false),
	mHasExtra(false),
	mHasString(false),
	mHasUnknown(false),
	mType(0),
	mData1(NULL),
	mData2(NULL),
	mData3(NULL),
	mData4(NULL)
{
}

bool cRZMessage2Standard::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZMessage2)
	{
		*outPtr = static_cast<cIGZMessage2*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZMessage2Standard)
	{
		*outPtr = static_cast<cIGZMessage2Standard*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cRZMessage2Standard::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cRZMessage2Standard::Release(void)
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}
	else
	{
		delete this;
		return 0;
	}
}

bool cRZMessage2Standard::Create(GZIID iid, void** outPtr) const
{
	cRZMessage2Standard* newMsg = new cRZMessage2Standard();
	if (newMsg->QueryInterface(iid, outPtr))
	{
		newMsg->SetType(GetType());
		return true;
	}
	else
	{
		delete newMsg;
		return false;
	}
}

GZGUID cRZMessage2Standard::GetType() const
{
	return mType;
}

void cRZMessage2Standard::SetType(GZGUID type)
{
	mType = type;
	if (type == 0)
	{
		mHasType = false;
	}
	else
	{
		mHasType = true;
	}
}

bool cRZMessage2Standard::operator== (cIGZMessage2 const& other) const
{
	return mType == other.GetType();
}

bool cRZMessage2Standard::operator< (cIGZMessage2 const& other) const
{
	return mType < other.GetType();
}

bool cRZMessage2Standard::GetHasData1() const
{
	return mHasData1 != 0;
}

uintptr_t cRZMessage2Standard::GetData1() const
{
	return (uintptr_t)mData1;
}

void* cRZMessage2Standard::GetVoid1() const
{
	return mData1;
}

void cRZMessage2Standard::SetData1(uintptr_t data)
{
	mData1 = reinterpret_cast<void*>(data);
	mHasData1 = true;
}

void cRZMessage2Standard::SetVoid1(void* data)
{
	mData1 = data;
	mHasData1 = true;
}

bool cRZMessage2Standard::GetHasData2() const
{
	return mHasData2 != 0;
}

uintptr_t cRZMessage2Standard::GetData2() const
{
	return (uintptr_t)mData2;
}

void* cRZMessage2Standard::GetVoid2() const
{
	return mData2;
}

void cRZMessage2Standard::SetData2(uintptr_t data)
{
	mData2 = reinterpret_cast<void*>(data);
	mHasData2 = true;
}

void cRZMessage2Standard::SetVoid2(void* data)
{
	mData2 = data;
	mHasData2 = true;
}

bool cRZMessage2Standard::GetHasData3() const
{
	return mHasData3 != 0;
}

uintptr_t cRZMessage2Standard::GetData3() const
{
	return (uintptr_t)mData3;
}

void* cRZMessage2Standard::GetVoid3() const
{
	return mData3;
}

void cRZMessage2Standard::SetData3(uintptr_t data)
{
	mData3 = reinterpret_cast<void*>(data);
	mHasData3 = true;
}

void cRZMessage2Standard::SetVoid3(void* data)
{
	mData3 = data;
	mHasData3 = true;
}

bool cRZMessage2Standard::GetHasData4() const
{
	return mHasData4 != 0;
}

uintptr_t cRZMessage2Standard::GetData4() const
{
	return (uintptr_t)mData4;
}

void* cRZMessage2Standard::GetVoid4() const
{
	return mData4;
}

void cRZMessage2Standard::SetData4(uintptr_t data)
{
	mData4 = reinterpret_cast<void*>(data);
	mHasData4 = true;
}

void cRZMessage2Standard::SetVoid4(void* data)
{
	mData4 = data;
	mHasData4 = true;
}

bool cRZMessage2Standard::GetHasString() const
{
	return mHasString != 0;
}

bool cRZMessage2Standard::GetString(GZIID iid, void** outPtr)
{
	if (GetHasString())
	{
		return mStr->QueryInterface(iid, outPtr);
	}
	else
	{
		return false;
	}
}

void cRZMessage2Standard::SetString(cIGZString* newStr)
{
	if (newStr != NULL)
	{
		mHasString = true;
	}
	else
	{
		mHasString = false;
	}

	mStr = newStr;
}

bool cRZMessage2Standard::GetHasIGZUnknown() const
{
	return mHasUnknown != 0;
}

cIGZUnknown* cRZMessage2Standard::GetIGZUnknown()
{
	return mUnknown;
}

void cRZMessage2Standard::SetIGZUnknown(cIGZUnknown* newUnknown)
{
	if (newUnknown != NULL)
	{
		mHasUnknown = true;
	}
	else
	{
		mHasUnknown = false;
	}

	mUnknown = newUnknown;
}

bool cRZMessage2Standard::GetHasExtra() const
{
	return mHasExtra != 0;
}

bool cRZMessage2Standard::GetExtra(GZIID iid, void** outPtr)
{
	if (GetHasExtra())
	{
		return mExtra->QueryInterface(iid, outPtr);
	}
	else
	{
		return false;
	}
}

void cRZMessage2Standard::SetExtra(cIGZSerializable* newExtra)
{
	if (newExtra != NULL)
	{
		mHasExtra = true;
	}
	else
	{
		mHasExtra = false;
	}

	mExtra = newExtra;
}