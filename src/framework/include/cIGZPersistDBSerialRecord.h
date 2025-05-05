/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

static const GZIID GZIID_cIGZPersistDBSerialRecord = 0xC56B8F08;

struct cGZPersistResourceKey;
class cIGZPersistDBRecord;
class cIGZString;
class cIGZVariant;

class cIGZPersistDBSerialRecord : public cIGZUnknown
{
public:
	virtual cIGZPersistDBRecord* AsIGZPersistDBRecord(void) = 0;

	virtual bool GetFieldUint8(uint8_t& value) = 0;
	virtual bool GetFieldUint8(uint8_t* array, size_t length) = 0;

	virtual bool GetFieldSint8(int8_t& value) = 0;
	virtual bool GetFieldSint8(int8_t* array, size_t length) = 0;

	virtual bool GetFieldUint16(uint16_t& value) = 0;
	virtual bool GetFieldUint16(uint16_t* array, size_t length) = 0;

	virtual bool GetFieldSint16(int16_t& value) = 0;
	virtual bool GetFieldSint16(int16_t* array, size_t length) = 0;

	virtual bool GetFieldUint32(uint32_t& value) = 0;
	virtual bool GetFieldUint32(uint32_t* array, size_t length) = 0;

	virtual bool GetFieldSint32(int32_t& value) = 0;
	virtual bool GetFieldSint32(int32_t* array, size_t length) = 0;

	virtual bool GetFieldUint64(uint64_t& value) = 0;
	virtual bool GetFieldUint64(uint64_t* array, size_t length) = 0;

	virtual bool GetFieldSint64(int64_t& value) = 0;
	virtual bool GetFieldSint64(int64_t* array, size_t length) = 0;

	virtual bool GetFieldFloat32(float& value) = 0;
	virtual bool GetFieldFloat32(float* array, size_t length) = 0;

	virtual bool GetFieldFloat64(double& value) = 0;
	virtual bool GetFieldFloat64(double* array, size_t length) = 0;

	virtual bool GetFieldString(cIGZString& string) = 0;
	virtual bool GetFieldResKey(cGZPersistResourceKey& key) = 0;
	virtual bool GetFieldVariant(cIGZVariant** outPtr) = 0;

	virtual bool GetFieldVoid(void* data, size_t length) = 0;

	virtual bool SetFieldUint8(uint8_t value) = 0;
	virtual bool SetFieldUint8(uint8_t const* array, size_t length) = 0;

	virtual bool SetFieldSint8(int8_t value) = 0;
	virtual bool SetFieldSint8(int8_t const* array, size_t length) = 0;

	virtual bool SetFieldUint16(uint16_t value) = 0;
	virtual bool SetFieldUint16(uint16_t const* array, size_t length) = 0;

	virtual bool SetFieldSint16(int16_t value) = 0;
	virtual bool SetFieldSint16(int16_t const* array, size_t length) = 0;

	virtual bool SetFieldUint32(uint32_t value) = 0;
	virtual bool SetFieldUint32(uint32_t const* array, size_t length) = 0;

	virtual bool SetFieldSint32(int32_t value) = 0;
	virtual bool SetFieldSint32(int32_t const* array, size_t length) = 0;

	virtual bool SetFieldUint64(uint64_t value) = 0;
	virtual bool SetFieldUint64(uint64_t const* array, size_t length) = 0;

	virtual bool SetFieldSint64(int64_t value) = 0;
	virtual bool SetFieldSint64(int64_t const* array, size_t length) = 0;

	virtual bool SetFieldFloat32(float value) = 0;
	virtual bool SetFieldFloat32(float const* array, size_t length) = 0;

	virtual bool SetFieldFloat64(double value) = 0;
	virtual bool SetFieldFloat64(double const* array, size_t length) = 0;

	virtual bool SetFieldString(cIGZString const& string) = 0;
	virtual bool SetFieldResKey(cGZPersistResourceKey const& key) = 0;
	virtual bool SetFieldVariant(cIGZVariant const* variant) = 0;

	virtual bool SetFieldVoid(void const* data, size_t length) = 0;

	virtual size_t GetSize(void) = 0;
	virtual void SetSize(size_t size) = 0;

	virtual uint32_t GetPosition(void) = 0;
	virtual bool SeekAbsolute(uint32_t position) = 0;
	virtual bool SeekRelative(int32_t offset) = 0;
};