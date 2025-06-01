/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "cGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cRZIStream.h"
#include "cRZOStream.h"

class cGZPersistDBSerialRecord :
	public cIGZPersistDBSerialRecord,
	public cGZPersistDBRecord,
	public cRZOStream,
	public cRZIStream
{
public:
	cGZPersistDBSerialRecord(cIGZPersistDBSegment* segment, bool useLittleEndian);
	cGZPersistDBSerialRecord(cGZPersistResourceKey const& key, cIGZPersistDBSegment* segment, bool useLittleEndian);
	virtual ~cGZPersistDBSerialRecord(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual cIGZPersistDBRecord* AsIGZPersistDBRecord(void);

	virtual bool GetFieldUint8(uint8_t& value);
	virtual bool GetFieldUint8(uint8_t* array, size_t length);

	virtual bool GetFieldSint8(int8_t& value);
	virtual bool GetFieldSint8(int8_t* array, size_t length);

	virtual bool GetFieldUint16(uint16_t& value);
	virtual bool GetFieldUint16(uint16_t* array, size_t length);

	virtual bool GetFieldSint16(int16_t& value);
	virtual bool GetFieldSint16(int16_t* array, size_t length);

	virtual bool GetFieldUint32(uint32_t& value);
	virtual bool GetFieldUint32(uint32_t* array, size_t length);

	virtual bool GetFieldSint32(int32_t& value);
	virtual bool GetFieldSint32(int32_t* array, size_t length);

	virtual bool GetFieldUint64(uint64_t& value);
	virtual bool GetFieldUint64(uint64_t* array, size_t length);

	virtual bool GetFieldSint64(int64_t& value);
	virtual bool GetFieldSint64(int64_t* array, size_t length);

	virtual bool GetFieldFloat32(float& value);
	virtual bool GetFieldFloat32(float* array, size_t length);

	virtual bool GetFieldFloat64(double& value);
	virtual bool GetFieldFloat64(double* array, size_t length);

	virtual bool GetFieldString(cIGZString& string);
	virtual bool GetFieldResKey(cGZPersistResourceKey& key);
	virtual bool GetFieldVariant(cIGZVariant** outPtr);

	virtual bool GetFieldVoid(void* data, size_t length) = 0;

	virtual bool SetFieldUint8(uint8_t value);
	virtual bool SetFieldUint8(uint8_t const* array, size_t length);

	virtual bool SetFieldSint8(int8_t value);
	virtual bool SetFieldSint8(int8_t const* array, size_t length);

	virtual bool SetFieldUint16(uint16_t value);
	virtual bool SetFieldUint16(uint16_t const* array, size_t length);

	virtual bool SetFieldSint16(int16_t value);
	virtual bool SetFieldSint16(int16_t const* array, size_t length);

	virtual bool SetFieldUint32(uint32_t value);
	virtual bool SetFieldUint32(uint32_t const* array, size_t length);

	virtual bool SetFieldSint32(int32_t value);
	virtual bool SetFieldSint32(int32_t const* array, size_t length);

	virtual bool SetFieldUint64(uint64_t value);
	virtual bool SetFieldUint64(uint64_t const* array, size_t length);

	virtual bool SetFieldSint64(int64_t value);
	virtual bool SetFieldSint64(int64_t const* array, size_t length);

	virtual bool SetFieldFloat32(float value);
	virtual bool SetFieldFloat32(float const* array, size_t length);

	virtual bool SetFieldFloat64(double value);
	virtual bool SetFieldFloat64(double const* array, size_t length);

	virtual bool SetFieldString(cIGZString const& string);
	virtual bool SetFieldResKey(cGZPersistResourceKey const& key);
	virtual bool SetFieldVariant(cIGZVariant const* variant);

	virtual bool SetFieldVoid(void const* data, size_t length) = 0;

	virtual uint32_t GetSize(void) = 0;
	virtual bool SetSize(size_t size) = 0;

	virtual uint32_t GetPosition(void) = 0;
	virtual bool SeekAbsolute(uint32_t position);
	virtual bool SeekRelative(int32_t offset);

public:
	virtual cGZPersistResourceKey GetKey(void);

public:
	virtual bool Skip(uint32_t bytes);

	virtual bool GetVoid(void* buffer, uint32_t size);
	virtual bool GetFieldVoid(void* data, uint32_t count, uint32_t elemSize);

	virtual bool SetVoid(void const* buffer, uint32_t size);
	virtual bool SetFieldVoid(void const* data, uint32_t count, uint32_t elemSize);

	virtual bool GetError(void) const;
	virtual void Flush(void);

	virtual bool SetRZCharStr(char const* str);
	virtual bool SetGZStr(cIGZString const& str);

	virtual cIGZOStream& operator<< (char const* str);
	virtual cIGZOStream& operator<< (cIGZString const& str);

	virtual bool GetRZCharStr(char* buffer, uint32_t size);
	virtual bool GetGZStr(cIGZString& out);

	virtual cIGZIStream& operator>> (cIGZString& str);

protected:
	uint16_t refCount;
	cGZPersistResourceKey key;
	bool useLittleEndian;
};