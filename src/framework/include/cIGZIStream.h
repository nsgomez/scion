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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZIStream = 0x051B11BD;

class cIGZSerializable;
class cIGZString;
class cIGZVariant;

class cIGZIStream : public cIGZUnknown
{
public:
	virtual bool Skip(uint32_t bytes) = 0;

	virtual bool GetSint8(int8_t& out) = 0;
	virtual bool GetUint8(uint8_t& out) = 0;
	virtual bool GetSint16(int16_t& out) = 0;
	virtual bool GetUint16(uint16_t& out) = 0;
	virtual bool GetSint32(int32_t& out) = 0;
	virtual bool GetUint32(uint32_t& out) = 0;
	virtual bool GetSint64(int64_t& out) = 0;
	virtual bool GetUint64(uint64_t& out) = 0;
	virtual bool GetFloat32(float& out) = 0;
	virtual bool GetFloat64(double& out) = 0;

	virtual bool GetRZCharStr(char* buffer, uint32_t size) = 0;
	virtual bool GetGZStr(cIGZString& out) = 0;
	virtual bool GetGZSerializable(cIGZSerializable& out) = 0;
	virtual bool GetVoid(void* buffer, uint32_t size) = 0;

	virtual bool GetError() const = 0;

	virtual void SetUserData(cIGZVariant* data) = 0;
	virtual cIGZVariant* GetUserData() = 0;
};