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
#include "cIGZCompression.h"

static const GZGUID GZCLSID_cRZFastCompression3 = 0x035990A3;

class cRZFastCompression3 : public cIGZCompression
{
public:
	cRZFastCompression3(void);
	virtual ~cRZFastCompression3(void) { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual bool CompressData(void const* data, uint32_t size, uint8_t* compressedData, uint32_t& compressedSize);
	virtual bool DecompressData(uint8_t const* data, uint32_t size, void* decompressedData, uint32_t& decompressedSize);
	virtual uint32_t GetMaxLengthRequiredForCompressedData(uint32_t size);
	virtual uint32_t GetLengthOfCompressedData(uint8_t const* data);
	virtual uint32_t GetLengthOfDecompressedData(uint8_t const* data);
	virtual void GetCompressionVersionString(cIGZString& str);

public:
	void EnableQuickCompression(bool flag);
	bool ValidateCompressedData(uint8_t const* data, uint32_t size);

protected:
	static const uint32_t kMaxRefPackDataLength = 0x1000000;

protected:
	uint32_t refCount;
	bool quickCompressionEnabled;
};