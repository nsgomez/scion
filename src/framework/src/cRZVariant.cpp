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

#include "cIGZString.h"
#include "cRZString.h"
#include "cRZVariant.h"

cRZVariant::cRZVariant()
{
	mData.Uint64 = 0;
	mRefCount = 0;
	mTypeTag = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(cRZVariant const& other)
{
	mData.Uint64 = 0;
	mRefCount = 0;
	mTypeTag = 0;
	mArrayCount = 0;

	*this = other;
}

cRZVariant::cRZVariant(bool value)
{
	mData.Uint64 = 0;
	mData.Bool = value;

	mTypeTag = tagRZVariant::Bool;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(uint8_t value)
{
	mData.Uint64 = 0;
	mData.Uint8 = value;

	mTypeTag = tagRZVariant::Uint8;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(int8_t value)
{
	mData.Uint64 = 0;
	mData.Sint8 = value;

	mTypeTag = tagRZVariant::Sint8;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(uint16_t value)
{
	mData.Uint64 = 0;
	mData.Uint16 = value;

	mTypeTag = tagRZVariant::Uint16;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(int16_t value)
{
	mData.Uint64 = 0;
	mData.Sint16 = value;

	mTypeTag = tagRZVariant::Sint16;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(uint32_t value)
{
	mData.Uint64 = 0;
	mData.Uint32 = value;

	mTypeTag = tagRZVariant::Uint32;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(int32_t value)
{
	mData.Uint64 = 0;
	mData.Sint32 = value;

	mTypeTag = tagRZVariant::Sint32;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(uint64_t value)
{
	mData.Uint64 = 0;
	mData.Uint64 = value;

	mTypeTag = tagRZVariant::Uint64;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(int64_t value)
{
	mData.Uint64 = 0;
	mData.Sint64 = value;

	mTypeTag = tagRZVariant::Sint64;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(float value)
{
	mData.Uint64 = 0;
	mData.Float32 = value;

	mTypeTag = tagRZVariant::Float32;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(double value)
{
	mData.Float64 = value;

	mTypeTag = tagRZVariant::Float64;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(cIGZString const& value)
{
	mData.Uint64 = 0;
	mTypeTag = 0;
	mRefCount = 0;
	mArrayCount = 0;

	RefRZChar(value.Data(), value.Strlen());
}

cRZVariant::cRZVariant(void const* value)
{
	mData.Uint64 = 0;
	mData.VoidPtr = const_cast<void*>(value);

	mTypeTag = tagRZVariant::VoidPtr;
	mRefCount = 0;
	mArrayCount = 0;
}

cRZVariant::cRZVariant(cIGZUnknown* value)
{
	mData.Uint64 = 0;
	mTypeTag = 0;
	mRefCount = 0;
	mArrayCount = 0;

	SetValUnknown(value);
}

cRZVariant::cRZVariant(void* src, uint32_t size, uint32_t tag)
{
	mData.Uint64 = 0;
	mTypeTag = 0;
	mRefCount = 0;
	mArrayCount = 0;

	RefVoid(src, size);
	mTypeTag = ((uint16_t)tag | tagRZVariant::Void);
}

bool cRZVariant::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	case GZIID_cIGZVariant:
		*outPtr = static_cast<cIGZVariant*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZVariant::AddRef()
{
	return ++mRefCount;
}

uint32_t cRZVariant::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

tagRZVariant::Type cRZVariant::GetType() const
{
	return (tagRZVariant::Type)mTypeTag;
}

uint32_t cRZVariant::GetCount() const
{
	return mArrayCount;
}

bool cRZVariant::Erase()
{
	Destroy();
	return true;
}

bool cRZVariant::CopyFrom(cIGZVariant const& src)
{
	*this = src;
	return true;
}

bool cRZVariant::CreateCopy(cIGZVariant** dest) const
{
	cRZVariant* newObj = new cRZVariant(*this);
	newObj->AddRef();
	*dest = newObj;

	return true;
}

bool cRZVariant::CanGetVal(tagRZVariant::Type valueType) const
{
	return true;
}

bool cRZVariant::CreateValString(cIGZString** dest) const
{
	cRZString* str = new cRZString();
	str->AddRef();

	if (cRZVariant::GetValString(*str))
	{
		*dest = str;
		return true;
	}

	str->Release();
	*dest = NULL;

	return false;
}

bool cRZVariant::GetValString(cIGZString& dest) const
{
	if (mTypeTag == (tagRZVariant::Array | tagRZVariant::RZChar))
	{
		dest.FromChar(static_cast<char const*>(mData.VoidPtr), mArrayCount);
		return true;
	}
	else if (mTypeTag == tagRZVariant::IGZUnknown)
	{
		cIGZString* str;
		if (mData.IGZUnknown->QueryInterface(GZIID_cIGZString, reinterpret_cast<void**>(&str)))
		{
			dest.FromChar(str->Data(), str->Strlen());
			str->Release();
			return true;
		}
	}

	cRZString str;
	ConvertToString(*this, str);
	dest.FromChar(str.Data(), (uint32_t)str.size());

	return false;
}

void cRZVariant::SetValString(cIGZString const& src)
{
	RefRZChar(src.Data(), src.Strlen());
}

bool cRZVariant::GetValVoid(void* dest, uint32_t& size) const
{
	if (mTypeTag == (tagRZVariant::Array | tagRZVariant::Void))
	{
		if (size > mArrayCount)
		{
			size = mArrayCount;
		}

		memmove(dest, mData.VoidPtr, size);
		return true;
	}

	return false;
}

void cRZVariant::SetValVoid(void const* src, uint32_t size)
{
	if (mTypeTag == (tagRZVariant::Array | tagRZVariant::Void) && mArrayCount == size && mData.VoidPtr != NULL)
	{
		memmove(mData.VoidPtr, src, size);
	}
	else
	{
		Destroy();
		mData.VoidPtr = new uint8_t[size];
		
		if (src != NULL)
		{
			memmove(mData.VoidPtr, src, size);
		}

		mArrayCount = size;
		mTypeTag = (tagRZVariant::Array | tagRZVariant::Void);
	}
}

bool cRZVariant::GetValUnknown(GZIID iid, void** dest)
{
	if (mTypeTag == tagRZVariant::IGZUnknown && mData.IGZUnknown != NULL)
	{
		return mData.IGZUnknown->QueryInterface(iid, dest);
	}
	else
	{
		return false;
	}
}

cIGZUnknown* cRZVariant::GetValUnknown()
{
	if (mTypeTag == tagRZVariant::IGZUnknown)
	{
		return mData.IGZUnknown;
	}

	return NULL;
}

void cRZVariant::SetValUnknown(cIGZUnknown* src)
{
	if (mTypeTag != tagRZVariant::IGZUnknown)
	{
		if (mTypeTag != tagRZVariant::Void)
		{
			Destroy();
		}

		mTypeTag = tagRZVariant::IGZUnknown;
	}

	if (src != mData.IGZUnknown)
	{
		if (src != NULL)
		{
			src->AddRef();
		}

		if (mData.IGZUnknown != NULL)
		{
			mData.IGZUnknown->Release();
		}

		mData.IGZUnknown = src;
	}
}

cIGZUnknown* cRZVariant::RefIGZUnknown() const
{
	return mData.IGZUnknown;
}

bool cRZVariant::RefIGZUnknown(GZIID iid, void** out)
{
	if (mTypeTag == tagRZVariant::IGZUnknown && mData.IGZUnknown != NULL)
	{
		return mData.IGZUnknown->QueryInterface(iid, out);
	}
	else
	{
		return false;
	}
}

void cRZVariant::RefIGZUnknown(cIGZUnknown* src)
{
	if (mTypeTag != tagRZVariant::IGZUnknown)
	{
		if (mTypeTag != tagRZVariant::Void)
		{
			Destroy();
		}

		mTypeTag = tagRZVariant::IGZUnknown;
	}

	if (src != mData.IGZUnknown)
	{
		if (src != NULL)
		{
			src->AddRef();
		}

		if (mData.IGZUnknown != NULL)
		{
			mData.IGZUnknown->Release();
		}

		mData.IGZUnknown = src;
	}
}

bool cRZVariant::ConvertTypes(cRZVariant const& src, cRZVariant& dest)
{
	// TODO
	return false;
}

void cRZVariant::Destroy()
{
	if (mTypeTag & tagRZVariant::Array)
	{
		delete mData.VoidPtr;
		mArrayCount = 0;
	}
	else if (mTypeTag == tagRZVariant::IGZUnknown)
	{
		mData.IGZUnknown->Release();
	}

	mData.VoidPtr = NULL;
	mData.Uint64 = 0;
	mTypeTag = 0;
	mArrayCount = 0;
}

#define IMPL_VARIANT_TYPE_REST(name, type) \
	type cRZVariant::GetVal ## name () const \
	{ \
		type val = 0; \
		GetVal ## name ## (val); \
		return val; \
	} \
	\
	void cRZVariant::SetVal ## name ## (type value) \
	{ \
		if (mTypeTag != tagRZVariant:: ## name ##) \
		{ \
			if (mTypeTag != tagRZVariant::Void) \
			{ \
				Destroy(); \
			} \
			\
			mTypeTag = tagRZVariant:: ## name ##; \
		} \
		\
		mData.name = value; \
	} \
	\
	type& cRZVariant::As ## name ## () \
	{ \
		return mData.name; \
	} \
	\
	type cRZVariant::As ## name ## () const \
	{ \
		return mData.name; \
	} \
	\
	type* cRZVariant::Ref ## name ## () const \
	{ \
		return reinterpret_cast<type*>(mData.VoidPtr); \
	} \
	\
	void cRZVariant::Ref ## name ## (type const* values, uint32_t count) \
	{ \
		if (mTypeTag == (tagRZVariant::Array | tagRZVariant:: ## name ##) && mArrayCount == count && mData.VoidPtr != NULL && values != NULL) \
		{ \
			memmove(mData.VoidPtr, values, count * sizeof(type)); \
		} \
		else \
		{ \
			Destroy(); \
			mData.VoidPtr = new type[count]; \
			if (values != NULL) \
			{ \
				memmove(mData.VoidPtr, values, count * sizeof(type)); \
			} \
			mArrayCount = count; \
			mTypeTag = (tagRZVariant::Array | tagRZVariant:: ## name ##); \
		} \
	}

#define IMPL_VARIANT_TYPE(name, type) \
	bool cRZVariant::GetVal ## name ## (type& dest) const \
	{ \
		if (mTypeTag == tagRZVariant:: ## name ##) \
		{ \
			dest = mData.name; \
			return true; \
		} \
		\
		cRZVariant tmp(static_cast<type>(0)); \
		bool result = false; \
		if (ConvertTypes(*this, tmp)) \
		{ \
			dest = tmp.mData.name; \
			result = true; \
		} \
		\
		return result; \
	} \
	\
	IMPL_VARIANT_TYPE_REST(name, type)

IMPL_VARIANT_TYPE(Bool, bool);
IMPL_VARIANT_TYPE(Uint8, uint8_t);
IMPL_VARIANT_TYPE(Sint8, int8_t);
IMPL_VARIANT_TYPE(Uint16, uint16_t);
IMPL_VARIANT_TYPE(Sint16, int16_t);
IMPL_VARIANT_TYPE(Uint32, uint32_t);
IMPL_VARIANT_TYPE(Sint32, int32_t);
IMPL_VARIANT_TYPE(Uint64, uint64_t);
IMPL_VARIANT_TYPE(Sint64, int64_t);
IMPL_VARIANT_TYPE_REST(Float32, float);
IMPL_VARIANT_TYPE_REST(Float64, double);
IMPL_VARIANT_TYPE_REST(Char, char);
IMPL_VARIANT_TYPE_REST(RZUnicodeChar, wchar_t);
IMPL_VARIANT_TYPE_REST(RZChar, char);
IMPL_VARIANT_TYPE(VoidPtr, void*);

// Void needs special handling
void* cRZVariant::RefVoid() const
{
	return mData.VoidPtr;
}

void cRZVariant::RefVoid(void const* values, uint32_t count)
{
	if (mTypeTag == (tagRZVariant::Array | tagRZVariant::Void) && mArrayCount == count && mData.VoidPtr != NULL && values != NULL)
	{
		memmove(mData.VoidPtr, values, count);
	}
	else
	{
		Destroy();
		mData.VoidPtr = new uint8_t[count];

		if (values != NULL)
		{
			memmove(mData.VoidPtr, values, count);
		}

		mArrayCount = count;
		mTypeTag = (tagRZVariant::Array | tagRZVariant::Void);
	}
}

// cRZVariant doesn't have constructors for char or wchar_t because they're ambiguous
// with the other types we have declared, so these are implemented without macros.
bool cRZVariant::GetValChar(char& dest) const
{
	if (mTypeTag == tagRZVariant::Char)
	{
		dest = mData.Char;
		return true;
	}

	cRZVariant tmp;
	tmp.mTypeTag = tagRZVariant::Char;
	tmp.mData.Char = 0;

	bool result = false;
	if (ConvertTypes(*this, tmp))
	{
		dest = tmp.mData.Char;
		result = true;
	}

	return result;
}

bool cRZVariant::GetValRZUnicodeChar(wchar_t& dest) const
{
	if (mTypeTag == tagRZVariant::RZUnicodeChar)
	{
		dest = mData.RZUnicodeChar;
		return true;
	}

	cRZVariant tmp;
	tmp.mTypeTag = tagRZVariant::RZUnicodeChar;
	tmp.mData.RZUnicodeChar = 0;

	bool result = false;
	if (ConvertTypes(*this, tmp))
	{
		dest = tmp.mData.RZUnicodeChar;
		result = true;
	}

	return result;
}

bool cRZVariant::GetValRZChar(char& dest) const
{
	if (mTypeTag == tagRZVariant::RZChar)
	{
		dest = mData.RZChar;
		return true;
	}

	cRZVariant tmp;
	tmp.mTypeTag = tagRZVariant::RZChar;
	tmp.mData.RZChar = 0;

	bool result = false;
	if (ConvertTypes(*this, tmp))
	{
		dest = tmp.mData.RZChar;
		result = true;
	}

	return result;
}

// FUTURE: allegedly, GetValFloat* use different implementations that leave
// `bool result` uninitialized and sets it in each branch. This is just for
// codegen accuracy and should be reconciled later.
bool cRZVariant::GetValFloat32(float& dest) const
{
	if (mTypeTag == tagRZVariant::Float32)
	{
		dest = mData.Float32;
		return true;
	}

	cRZVariant tmp(static_cast<float>(0));
	bool result;

	if (ConvertTypes(*this, tmp))
	{
		dest = tmp.mData.Float32;
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}

bool cRZVariant::GetValFloat64(double& dest) const
{
	if (mTypeTag == tagRZVariant::Float64)
	{
		dest = mData.Float64;
		return true;
	}

	cRZVariant tmp(static_cast<double>(0));
	bool result;

	if (ConvertTypes(*this, tmp))
	{
		dest = tmp.mData.Float64;
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}