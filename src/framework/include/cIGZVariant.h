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

#pragma once
#include "cIGZUnknown.h"

static const uint32_t GZIID_cIGZVariant = 0x60FD4D2B;

#define DECL_VARIANT_TYPE(name, type) \
	virtual bool GetVal ## name ## (type& dest) const = 0; \
	virtual type GetVal ## name ## () const = 0; \
	virtual void SetVal ## name ## (type value) = 0

#define DECL_VARIANT_RETURN_AS(name, type) \
	virtual type& As ## name ## () = 0; \
	virtual type As ## name ## () const = 0

#define DECL_VARIANT_RETURN_AS_REF(name, type) \
	virtual type* Ref ## name ## () const = 0; \
	virtual void Ref ## name ## (type const* values, uint32_t count) = 0

class cIGZString;

struct tagRZVariant
{
	typedef enum
	{
		Void = 0,
		Bool = 1,
		Uint8 = 2,
		Sint8 = 3,
		Uint16 = 4,
		Sint16 = 5,
		Uint32 = 6,
		Sint32 = 7,
		Uint64 = 8,
		Sint64 = 9,
		Float32 = 10,
		Float64 = 11,
		Char = 12,
		RZUnicodeChar = 13,
		RZChar = 14,
		VoidPtr = 15,
		IGZUnknown = 16,
		Array = 0x8000,
	} Type;
};

class cIGZVariant : public cIGZUnknown
{
public:
	virtual tagRZVariant::Type GetType() const = 0;
	virtual uint32_t GetCount() const = 0;

	virtual bool Erase() = 0;
	virtual bool CopyFrom(cIGZVariant const& src) = 0;
	virtual bool CreateCopy(cIGZVariant** dest) const = 0;

	virtual bool CanGetVal(tagRZVariant::Type valueType) const = 0;

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

	virtual bool CreateValString(cIGZString** dest) const = 0;
	virtual bool GetValString(cIGZString& dest) const = 0;
	virtual void SetValString(cIGZString const& src) = 0;

	virtual bool GetValVoid(void* dest, uint32_t& size) const = 0;
	virtual void SetValVoid(void const* src, uint32_t size) = 0;

	virtual bool GetValUnknown(GZIID iid, void** dest) = 0;
	virtual cIGZUnknown* GetValUnknown() = 0;
	virtual void SetValUnknown(cIGZUnknown* src) = 0;

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

	virtual void* AsVoidPtr() = 0;
	virtual void* AsVoidPtr() const = 0;

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

	virtual cIGZUnknown* RefIGZUnknown() const = 0;
	virtual bool RefIGZUnknown(GZIID iid, void** out) = 0;
	virtual void RefIGZUnknown(cIGZUnknown* src) = 0;

	virtual ~cIGZVariant();
};

#undef DECL_VARIANT_RETURN_AS
#undef DECL_VARIANT_RETURN_AS_REF
#undef DECL_VARIANT_TYPE