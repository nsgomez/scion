/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2022  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cIGZSerializable.h"
#include "cIGZString.h"
#include "cRZMessage2Standard.h"

// TODO: messages have a custom allocator on W32, but just use malloc on POSIX
// Need to figure out how to override `operator new` and `operator delete`

cRZMessage2Standard::cRZMessage2Standard() :
	refCount(0),
	hasType(false),
	hasData1(false),
	hasData2(false),
	hasData3(false),
	hasData4(false),
	hasExtra(false),
	hasString(false),
	hasUnknown(false),
	type(0),
	data1(NULL),
	data2(NULL),
	data3(NULL),
	data4(NULL),
	str(NULL),
	unknown(NULL),
	extra(NULL)
{
}

cRZMessage2Standard::~cRZMessage2Standard()
{
	if (extra != NULL)
	{
		extra->Release();
	}

	if (unknown != NULL)
	{
		unknown->Release();
	}

	if (str != NULL)
	{
		str->Release();
	}
}

bool cRZMessage2Standard::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZMessage2Standard)
	{
		*outPtr = static_cast<cIGZMessage2Standard*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZMessage2)
	{
		*outPtr = static_cast<cIGZMessage2*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
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
	return ++refCount;
}

uint32_t cRZMessage2Standard::Release(void)
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

bool cRZMessage2Standard::Create(GZIID iid, void** outPtr) const
{
	cRZMessage2Standard* newMsg = new cRZMessage2Standard();
	if (iid == GZIID_cIGZUnknown || iid == GZIID_cIGZMessage2 || iid == GZIID_cIGZMessage2Standard)
	{
		*outPtr = newMsg;
		newMsg->refCount = 1;

		GZGUID newType = GetType();
		newMsg->type = newType;
		newMsg->hasType = (newType != 0);

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
	return type;
}

void cRZMessage2Standard::SetType(GZGUID type)
{
	this->type = type;
	hasType = (type != 0);
}

bool cRZMessage2Standard::operator== (cIGZMessage2 const& other) const
{
	return this->type == other.GetType();
}

bool cRZMessage2Standard::operator< (cIGZMessage2 const& other) const
{
	return this->type < other.GetType();
}

bool cRZMessage2Standard::GetHasData1() const
{
	return hasData1 != 0;
}

uintptr_t cRZMessage2Standard::GetData1() const
{
	return (uintptr_t)data1;
}

void* cRZMessage2Standard::GetVoid1() const
{
	return data1;
}

void cRZMessage2Standard::SetData1(uintptr_t data)
{
	data1 = reinterpret_cast<void*>(data);
	hasData1 = true;
}

void cRZMessage2Standard::SetVoid1(void* data)
{
	data1 = data;
	hasData1 = true;
}

bool cRZMessage2Standard::GetHasData2() const
{
	return hasData2 != 0;
}

uintptr_t cRZMessage2Standard::GetData2() const
{
	return (uintptr_t)data2;
}

void* cRZMessage2Standard::GetVoid2() const
{
	return data2;
}

void cRZMessage2Standard::SetData2(uintptr_t data)
{
	data2 = reinterpret_cast<void*>(data);
	hasData2 = true;
}

void cRZMessage2Standard::SetVoid2(void* data)
{
	data2 = data;
	hasData2 = true;
}

bool cRZMessage2Standard::GetHasData3() const
{
	return hasData3 != 0;
}

uintptr_t cRZMessage2Standard::GetData3() const
{
	return (uintptr_t)data3;
}

void* cRZMessage2Standard::GetVoid3() const
{
	return data3;
}

void cRZMessage2Standard::SetData3(uintptr_t data)
{
	data3 = reinterpret_cast<void*>(data);
	hasData3 = true;
}

void cRZMessage2Standard::SetVoid3(void* data)
{
	data3 = data;
	hasData3 = true;
}

bool cRZMessage2Standard::GetHasData4() const
{
	return hasData4 != 0;
}

uintptr_t cRZMessage2Standard::GetData4() const
{
	return (uintptr_t)data4;
}

void* cRZMessage2Standard::GetVoid4() const
{
	return data4;
}

void cRZMessage2Standard::SetData4(uintptr_t data)
{
	data4 = reinterpret_cast<void*>(data);
	hasData4 = true;
}

void cRZMessage2Standard::SetVoid4(void* data)
{
	data4 = data;
	hasData4 = true;
}

bool cRZMessage2Standard::GetHasString() const
{
	return hasString != 0;
}

bool cRZMessage2Standard::GetString(GZIID iid, void** outPtr)
{
	if (!GetHasString())
	{
		return false;
	}
	else
	{
		return str->QueryInterface(iid, outPtr);
	}
}

void cRZMessage2Standard::SetString(cIGZString* newStr)
{
	hasString = newStr != NULL;

	cIGZString* oldStr = str;
	if (oldStr != newStr)
	{
		if (newStr != NULL)
		{
			newStr->AddRef();
		}

		str = newStr;
		if (oldStr != NULL)
		{
			oldStr->Release();
		}
	}
}

bool cRZMessage2Standard::GetHasIGZUnknown() const
{
	return hasUnknown != 0;
}

cIGZUnknown* cRZMessage2Standard::GetIGZUnknown()
{
	return unknown;
}

void cRZMessage2Standard::SetIGZUnknown(cIGZUnknown* newUnknown)
{
	hasUnknown = newUnknown != NULL;

	cIGZUnknown* oldUnknown = unknown;
	if (oldUnknown != newUnknown)
	{
		if (newUnknown != NULL)
		{
			newUnknown->AddRef();
		}

		unknown = newUnknown;
		if (oldUnknown != NULL)
		{
			oldUnknown->Release();
		}
	}
}

bool cRZMessage2Standard::GetHasExtra() const
{
	return hasExtra != 0;
}

bool cRZMessage2Standard::GetExtra(GZIID iid, void** outPtr)
{
	if (!GetHasExtra())
	{
		return false;
	}
	else
	{
		return extra->QueryInterface(iid, outPtr);
	}
}

void cRZMessage2Standard::SetExtra(cIGZSerializable* newExtra)
{
	hasExtra = newExtra != NULL;

	cIGZUnknown* oldExtra = extra;
	if (oldExtra != newExtra)
	{
		if (newExtra != NULL)
		{
			newExtra->AddRef();
		}

		extra = newExtra;
		if (oldExtra != NULL)
		{
			oldExtra->Release();
		}
	}
}