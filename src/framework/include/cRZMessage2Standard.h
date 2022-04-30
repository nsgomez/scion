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
#include "cIGZMessage2Standard.h"

const GZCLSID GZCLSID_cRZMessage2Standard = 0x46DF45E7;

class cRZMessage2Standard : public cIGZMessage2Standard
{
public:
	cRZMessage2Standard();
	virtual ~cRZMessage2Standard();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual bool Create(GZIID iid, void** outPtr) const;

	virtual GZGUID GetType() const;
	virtual void SetType(GZGUID type);

	virtual bool operator== (cIGZMessage2 const& other) const;
	virtual bool operator<  (cIGZMessage2 const& other) const;

public:
	virtual bool GetHasData1() const;
	virtual uintptr_t GetData1() const;
	virtual void* GetVoid1() const;
	virtual void SetData1(uintptr_t data);
	virtual void SetVoid1(void* data);

	virtual bool GetHasData2() const;
	virtual uintptr_t GetData2() const;
	virtual void* GetVoid2() const;
	virtual void SetData2(uintptr_t data);
	virtual void SetVoid2(void* data);

	virtual bool GetHasData3() const;
	virtual uintptr_t GetData3() const;
	virtual void* GetVoid3() const;
	virtual void SetData3(uintptr_t data);
	virtual void SetVoid3(void* data);

	virtual bool GetHasData4() const;
	virtual uintptr_t GetData4() const;
	virtual void* GetVoid4() const;
	virtual void SetData4(uintptr_t data);
	virtual void SetVoid4(void* data);

	virtual bool GetHasString() const;
	virtual bool GetString(GZIID iid, void** outPtr);
	virtual void SetString(cIGZString* newStr);

	virtual bool GetHasIGZUnknown() const;
	virtual cIGZUnknown* GetIGZUnknown();
	virtual void SetIGZUnknown(cIGZUnknown* newUnknown);

	virtual bool GetHasExtra() const;
	virtual bool GetExtra(GZIID iid, void** outPtr);
	virtual void SetExtra(cIGZSerializable* newExtra);

protected:
	enum RZMessage2StandardFlags
	{
		kMessageHasType    = (1 << 0),
		kMessageHasData1   = (1 << 1),
		kMessageHasData2   = (1 << 2),
		kMessageHasData3   = (1 << 3),
		kMessageHasData4   = (1 << 4),
		kMessageHasExtra   = (1 << 5),
		kMessageHasString  = (1 << 6),
		kMessageHasUnknown = (1 << 7),
	};

	uint32_t refCount;

	uint8_t hasType    : 1;
	uint8_t hasData1   : 1;
	uint8_t hasData2   : 1;
	uint8_t hasData3   : 1;
	uint8_t hasData4   : 1;
	uint8_t hasExtra   : 1;
	uint8_t hasString  : 1;
	uint8_t hasUnknown : 1;

	GZGUID type;
	void* data1;
	void* data2;
	void* data3;
	void* data4;
	cIGZString* str;
	cIGZUnknown* unknown;
	cIGZSerializable* extra;
};