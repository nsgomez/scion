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
#include "cRZVariant.h"

cRZVariant::cRZVariant()
{
}

cRZVariant::cRZVariant(cRZVariant const& other)
{
	*this = other;
}

cRZVariant::cRZVariant(bool value)
{
	data.Bool = value;
	typeTag = tagRZVariant::Bool;
}

cRZVariant::cRZVariant(uint8_t value)
{
	data.Uint8 = value;
	typeTag = tagRZVariant::Uint8;
}

cRZVariant::cRZVariant(int8_t value)
{
	data.Sint8 = value;
	typeTag = tagRZVariant::Sint8;
}

cRZVariant::cRZVariant(uint16_t value)
{
	data.Uint16 = value;
	typeTag = tagRZVariant::Uint16;
}

cRZVariant::cRZVariant(int16_t value)
{
	data.Sint16 = value;
	typeTag = tagRZVariant::Sint16;
}

cRZVariant::cRZVariant(uint32_t value)
{
	data.Uint32 = value;
	typeTag = tagRZVariant::Uint32;
}

cRZVariant::cRZVariant(int32_t value)
{
	data.Sint32 = value;
	typeTag = tagRZVariant::Sint32;
}

cRZVariant::cRZVariant(uint64_t value)
{
	data.Uint64 = value;
	typeTag = tagRZVariant::Uint64;
}

cRZVariant::cRZVariant(int64_t value)
{
	data.Sint64 = value;
	typeTag = tagRZVariant::Sint64;
}

cRZVariant::cRZVariant(float value)
{
	data.Float32 = value;
	typeTag = tagRZVariant::Float32;
}

cRZVariant::cRZVariant(double value)
{
	data.Float64 = value;
	typeTag = tagRZVariant::Float64;
}

cRZVariant::cRZVariant(cIGZString const& value)
{
	RefRZChar(value.Data(), value.Strlen());
}

cRZVariant::cRZVariant(void const* value)
{
	data.VoidPtr = const_cast<void*>(value);
	typeTag = tagRZVariant::VoidPtr;
}

cRZVariant::cRZVariant(cIGZUnknown* value)
{
	typeTag = tagRZVariant::IGZUnknown;
	if (value != NULL)
	{
		value->AddRef();
		
		cIGZUnknown* currentValue = data.IGZUnknown;
		if (currentValue != NULL)
		{
			currentValue->Release();
		}

		data.VoidPtr = value;
	}
}

cRZVariant::cRZVariant(void* src, uint32_t size, uint32_t tag)
{
	RefVoid(src, size);
	typeTag = ((uint16_t)tag | tagRZVariant::Void);
}

cRZVariant::~cRZVariant()
{
	Destroy();
}

bool cRZVariant::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid = GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid = GZIID_cIGZVariant)
	{
		*outPtr = static_cast<cIGZVariant*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZVariant::AddRef()
{
	return ++refCount;
}

uint32_t cRZVariant::Release()
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

tagRZVariant::Type cRZVariant::GetType() const
{
	return (tagRZVariant::Type)typeTag;
}

uint32_t cRZVariant::GetCount() const
{
	return arrayCount;
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
	// TODO
	return false;
}

bool cRZVariant::GetValString(cIGZString& dest) const
{
	// TODO
	return false;
}

void cRZVariant::SetValString(cIGZString const& src)
{
	// TODO
}

bool cRZVariant::GetValVoid(void* dest, uint32_t& size) const
{
	if (typeTag == (tagRZVariant::Array | tagRZVariant::Void))
	{
		uint32_t count = this->arrayCount;
		if (count < size)
		{
			size = count;
		}

		memcpy(dest, data.VoidPtr, size);
		return true;
	}
	else
	{
		return false;
	}
}

void cRZVariant::SetValVoid(void const* src, uint32_t size)
{
	if (typeTag == (tagRZVariant::Array | tagRZVariant::Void) && arrayCount == size && data.VoidPtr != NULL)
	{
		memcpy(data.VoidPtr, src, size);
	}
	else
	{
		Destroy();
		data.VoidPtr = new uint8_t[size];
		
		if (src != NULL)
		{
			memcpy(data.VoidPtr, src, size);
		}

		arrayCount = size;
		typeTag = (tagRZVariant::Array | tagRZVariant::Void);
	}
}

bool cRZVariant::GetValUnknown(GZIID iid, void** dest)
{
	if (typeTag == tagRZVariant::IGZUnknown && data.IGZUnknown != NULL)
	{
		return data.IGZUnknown->QueryInterface(iid, dest);
	}
	else
	{
		return false;
	}
}

cIGZUnknown* cRZVariant::GetValUnknown()
{
	cIGZUnknown* result = NULL;
	if (typeTag == tagRZVariant::IGZUnknown)
	{
		result = data.IGZUnknown;
	}

	return result;
}

void cRZVariant::SetValUnknown(cIGZUnknown* src)
{
	if (typeTag != 0 && typeTag != tagRZVariant::IGZUnknown)
	{
		Destroy();
	}

	if (data.IGZUnknown != src)
	{
		if (src != NULL)
		{
			src->AddRef();
		}

		if (data.IGZUnknown != NULL)
		{
			data.IGZUnknown->Release();
		}

		data.IGZUnknown = src;
	}
}

void* cRZVariant::AsVoidPtr()
{
	return data.VoidPtr;
}

void* cRZVariant::AsVoidPtr() const
{
	return data.VoidPtr;
}

cIGZUnknown* cRZVariant::RefIGZUnknown() const
{
	return data.IGZUnknown;
}

bool cRZVariant::RefIGZUnknown(GZIID iid, void** out)
{
	if (typeTag == tagRZVariant::IGZUnknown && data.IGZUnknown != NULL)
	{
		return data.IGZUnknown->QueryInterface(iid, out);
	}
	else
	{
		return false;
	}
}

void cRZVariant::RefIGZUnknown(cIGZUnknown* src)
{
	if (typeTag != 0 && typeTag != tagRZVariant::IGZUnknown)
	{
		Destroy();
	}

	typeTag = tagRZVariant::IGZUnknown;
	cIGZUnknown* original = data.IGZUnknown;

	if (original != src)
	{
		if (src != NULL)
		{
			src->AddRef();
		}

		if (original != NULL)
		{
			original->Release();
		}

		data.IGZUnknown = src;
	}
}

bool cRZVariant::ConvertTypes(cRZVariant const& src, cRZVariant& dest)
{
	// TODO
	return false;
}

void cRZVariant::Destroy()
{
	if (typeTag >= tagRZVariant::Array)
	{
		if (data.VoidPtr != NULL)
		{
			delete data.VoidPtr;
		}

		data.VoidPtr = NULL;
		arrayCount = 0;
	}
	else if (typeTag == tagRZVariant::IGZUnknown)
	{
		cIGZUnknown* unknown = data.IGZUnknown;
		if (unknown != NULL)
		{
			unknown->Release();
		}

		data.IGZUnknown = NULL;
	}

	data.Uint64 = 0;
	typeTag = 0;
	arrayCount = 0;
}

#define IMPL_VARIANT_TYPE(name, type) \
	bool cRZVariant::GetVal ## name (type& dest) const \
	{ \
		if (this->typeTag == tagRZVariant:: ## name ##) \
		{ \
			dest = this->data.name; \
			return true; \
		} \
		else \
		{ \
			cRZVariant tmp; \
			tmp.typeTag = tagRZVariant:: ## name ##; \
			bool succeeded = ConvertTypes(*this, tmp); \
			if (succeeded) \
			{ \
				dest = tmp.data.name; \
			} \
			return succeeded; \
		} \
	} \
	\
	type cRZVariant::GetVal ## name () const \
	{ \
		type val; \
		GetVal ## name ## (val); \
		return val; \
	} \
	\
	void cRZVariant::SetVal ## name ## (type value) \
	{ \
		if (typeTag != tagRZVariant:: ## name ## && typeTag != tagRZVariant:: ## name ##) \
		{ \
			Destroy(); \
		} \
	} \
	\
	type& cRZVariant::As ## name ## () \
	{ \
		return data.name; \
	} \
	\
	type cRZVariant::As ## name ## () const \
	{ \
		return data.name; \
	} \
	\
	type* cRZVariant::Ref ## name ## () const \
	{ \
		return reinterpret_cast<type*>(data.VoidPtr); \
	} \
	\
	void cRZVariant::Ref ## name ## (type const* values, uint32_t count) \
	{ \
		if (typeTag == (tagRZVariant::Array | tagRZVariant:: ## name ##) && arrayCount == count && data.VoidPtr != NULL && values != NULL) \
		{ \
			memcpy(data.VoidPtr, values, count * sizeof(type)); \
		} \
		else \
		{ \
			Destroy(); \
			data.VoidPtr = new type[count]; \
			if (values != NULL) \
			{ \
				memcpy(data.VoidPtr, values, count * sizeof(type)); \
			} \
			arrayCount = count; \
			typeTag = (tagRZVariant::Array | tagRZVariant:: ## name ##); \
		} \
	}

IMPL_VARIANT_TYPE(Bool, bool);
IMPL_VARIANT_TYPE(Uint8, uint8_t);
IMPL_VARIANT_TYPE(Sint8, int8_t);
IMPL_VARIANT_TYPE(Uint16, uint16_t);
IMPL_VARIANT_TYPE(Sint16, int16_t);
IMPL_VARIANT_TYPE(Uint32, uint32_t);
IMPL_VARIANT_TYPE(Sint32, int32_t);
IMPL_VARIANT_TYPE(Uint64, uint64_t);
IMPL_VARIANT_TYPE(Sint64, int64_t);
IMPL_VARIANT_TYPE(Float32, float);
IMPL_VARIANT_TYPE(Float64, double);
IMPL_VARIANT_TYPE(Char, char);
IMPL_VARIANT_TYPE(RZUnicodeChar, wchar_t);
IMPL_VARIANT_TYPE(RZChar, char);