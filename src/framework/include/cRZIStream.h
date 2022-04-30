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
#include "cIGZIStream.h"

class cRZIStream : public cIGZIStream
{
public:
	cRZIStream(bool useLittleEndian);
	virtual ~cRZIStream();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool GetSint8(int8_t& out);
	virtual bool GetUint8(uint8_t& out);
	virtual bool GetSint16(int16_t& out);
	virtual bool GetUint16(uint16_t& out);
	virtual bool GetSint32(int32_t& out);
	virtual bool GetUint32(uint32_t& out);
	virtual bool GetSint64(int64_t& out);
	virtual bool GetUint64(uint64_t& out);
	virtual bool GetFloat32(float& out);
	virtual bool GetFloat64(double& out);

	virtual bool GetRZCharStr(char* buffer, uint32_t size);
	virtual bool GetGZStr(cIGZString& out);
	virtual bool GetGZSerializable(cIGZSerializable& out);

	virtual void SetUserData(cIGZVariant* data);
	virtual cIGZVariant* GetUserData();

public:
	virtual bool Init(bool useLittleEndian);
	virtual bool Shutdown();

protected:
	void DecodeStringLength(uint32_t& length);

	uint32_t refCount;
	bool useLittleEndian;
	cIGZVariant* userData;
};