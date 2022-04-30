/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#pragma once
#include "cIGZVariant.h"

#define DECL_VARIANT_TYPE(name, type) \
	virtual bool GetVal ## name ## (type& dest) const; \
	virtual type GetVal ## name ## () const; \
	virtual void SetVal ## name ## (type value)

#define DECL_VARIANT_RETURN_AS(name, type) \
	virtual type& As ## name ## (); \
	virtual type As ## name ## () const

#define DECL_VARIANT_RETURN_AS_REF(name, type) \
	virtual type* Ref ## name ## () const; \
	virtual void Ref ## name ## (type const* values, uint32_t count)

class cRZVariant : public cIGZVariant
{
public:
	cRZVariant();
	cRZVariant(cRZVariant const& other);

	cRZVariant(bool value);
	cRZVariant(uint8_t value);
	cRZVariant(int8_t value);
	cRZVariant(uint16_t value);
	cRZVariant(int16_t value);
	cRZVariant(uint32_t value);
	cRZVariant(int32_t value);
	cRZVariant(uint64_t value);
	cRZVariant(int64_t value);
	cRZVariant(float value);
	cRZVariant(double value);
	cRZVariant(cIGZString const& value);
	cRZVariant(void const* value);
	cRZVariant(cIGZUnknown* value);
	cRZVariant(void* src, uint32_t size, uint32_t tag);

	virtual ~cRZVariant();

	cRZVariant& operator= (cIGZVariant const& other);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual tagRZVariant::Type GetType() const;
	virtual uint32_t GetCount() const;

	virtual bool Erase();
	virtual bool CopyFrom(cIGZVariant const& src);
	virtual bool CreateCopy(cIGZVariant** dest) const;

	virtual bool CanGetVal(tagRZVariant::Type valueType) const;

	DECL_VARIANT_TYPE(Bool, bool);
	DECL_VARIANT_TYPE(Uint8, uint8_t);
	DECL_VARIANT_TYPE(Sint8, int8_t);
	DECL_VARIANT_TYPE(Uint16, uint16_t);
	DECL_VARIANT_TYPE(Sint16, int16_t);
	DECL_VARIANT_TYPE(Uint32, uint32_t);
	DECL_VARIANT_TYPE(Sint32, int32_t);
	DECL_VARIANT_TYPE(Uint64, uint64_t);
	DECL_VARIANT_TYPE(Sint64, int64_t);
	DECL_VARIANT_TYPE(Float32, float);
	DECL_VARIANT_TYPE(Float64, double);
	DECL_VARIANT_TYPE(Char, char);
	DECL_VARIANT_TYPE(RZUnicodeChar, wchar_t);
	DECL_VARIANT_TYPE(RZChar, char);
	DECL_VARIANT_TYPE(VoidPtr, void*);

	virtual bool CreateValString(cIGZString** dest) const;
	virtual bool GetValString(cIGZString& dest) const;
	virtual void SetValString(cIGZString const& src);

	virtual bool GetValVoid(void* dest, uint32_t& size) const;
	virtual void SetValVoid(void const* src, uint32_t size);

	virtual bool GetValUnknown(GZIID iid, void** dest);
	virtual cIGZUnknown* GetValUnknown();
	virtual void SetValUnknown(cIGZUnknown* src);

	DECL_VARIANT_RETURN_AS(Bool, bool);
	DECL_VARIANT_RETURN_AS(Uint8, uint8_t);
	DECL_VARIANT_RETURN_AS(Sint8, int8_t);
	DECL_VARIANT_RETURN_AS(Uint16, uint16_t);
	DECL_VARIANT_RETURN_AS(Sint16, int16_t);
	DECL_VARIANT_RETURN_AS(Uint32, uint32_t);
	DECL_VARIANT_RETURN_AS(Sint32, int32_t);
	DECL_VARIANT_RETURN_AS(Uint64, uint64_t);
	DECL_VARIANT_RETURN_AS(Sint64, int64_t);
	DECL_VARIANT_RETURN_AS(Float32, float);
	DECL_VARIANT_RETURN_AS(Float64, double);
	DECL_VARIANT_RETURN_AS(Char, char);
	DECL_VARIANT_RETURN_AS(RZUnicodeChar, wchar_t);
	DECL_VARIANT_RETURN_AS(RZChar, char);

	virtual void* AsVoidPtr();
	virtual void* AsVoidPtr() const;

	DECL_VARIANT_RETURN_AS_REF(Bool, bool);
	DECL_VARIANT_RETURN_AS_REF(Uint8, uint8_t);
	DECL_VARIANT_RETURN_AS_REF(Sint8, int8_t);
	DECL_VARIANT_RETURN_AS_REF(Uint16, uint16_t);
	DECL_VARIANT_RETURN_AS_REF(Sint16, int16_t);
	DECL_VARIANT_RETURN_AS_REF(Uint32, uint32_t);
	DECL_VARIANT_RETURN_AS_REF(Sint32, int32_t);
	DECL_VARIANT_RETURN_AS_REF(Uint64, uint64_t);
	DECL_VARIANT_RETURN_AS_REF(Sint64, int64_t);
	DECL_VARIANT_RETURN_AS_REF(Float32, float);
	DECL_VARIANT_RETURN_AS_REF(Float64, double);
	DECL_VARIANT_RETURN_AS_REF(Char, char);
	DECL_VARIANT_RETURN_AS_REF(RZUnicodeChar, wchar_t);
	DECL_VARIANT_RETURN_AS_REF(RZChar, char);
	DECL_VARIANT_RETURN_AS_REF(VoidPtr, void*);
	DECL_VARIANT_RETURN_AS_REF(Void, void);

	virtual cIGZUnknown* RefIGZUnknown() const;
	virtual bool RefIGZUnknown(GZIID iid, void** out);
	virtual void RefIGZUnknown(cIGZUnknown* src);

protected:
	static bool ConvertTypes(cRZVariant const& src, cRZVariant& dest);

	void Destroy();

protected:
	union
	{
		bool Bool;
		uint8_t Uint8;
		int8_t Sint8;
		uint16_t Uint16;
		int16_t Sint16;
		uint32_t Uint32;
		int32_t Sint32;
		uint64_t Uint64;
		int64_t Sint64;
		float Float32;
		double Float64;
		char Char;
		wchar_t RZUnicodeChar;
		char RZChar;
		void* VoidPtr;
		cIGZUnknown* IGZUnknown;
	} data;

	uint16_t refCount;
	uint16_t typeTag;
	uint32_t arrayCount;
};

#undef DECL_VARIANT_RETURN_AS
#undef DECL_VARIANT_RETURN_AS_REF
#undef DECL_VARIANT_TYPE