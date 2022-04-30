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

#pragma once
#include "cIGZMessage2.h"

static const GZIID GZIID_cIGZMessage2Standard = 0x65297976;

class cIGZSerializable;
class cIGZString;

class cIGZMessage2Standard : public cIGZMessage2
{
public:
	virtual bool GetHasData1() const = 0;
	virtual uintptr_t GetData1() const = 0;
	virtual void* GetVoid1() const = 0;
	virtual void SetData1(uintptr_t data) = 0;
	virtual void SetVoid1(void* data) = 0;

	virtual bool GetHasData2() const = 0;
	virtual uintptr_t GetData2() const = 0;
	virtual void* GetVoid2() const = 0;
	virtual void SetData2(uintptr_t data) = 0;
	virtual void SetVoid2(void* data) = 0;

	virtual bool GetHasData3() const = 0;
	virtual uintptr_t GetData3() const = 0;
	virtual void* GetVoid3() const = 0;
	virtual void SetData3(uintptr_t data) = 0;
	virtual void SetVoid3(void* data) = 0;

	virtual bool GetHasData4() const = 0;
	virtual uintptr_t GetData4() const = 0;
	virtual void* GetVoid4() const = 0;
	virtual void SetData4(uintptr_t data) = 0;
	virtual void SetVoid4(void* data) = 0;

	virtual bool GetHasString() const = 0;
	virtual bool GetString(GZIID iid, void** outPtr) = 0;
	virtual void SetString(cIGZString* str) = 0;

	virtual bool GetHasIGZUnknown() const = 0;
	virtual cIGZUnknown* GetIGZUnknown() = 0;
	virtual void SetIGZUnknown(cIGZUnknown* unknown) = 0;

	virtual bool GetHasExtra() const = 0;
	virtual bool GetExtra(GZIID iid, void** outPtr) = 0;
	virtual void SetExtra(cIGZSerializable* extra) = 0;
};