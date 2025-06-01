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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZCompression = 0x8359908E;

class cIGZString;

class cIGZCompression : public cIGZUnknown
{
public:
	virtual bool CompressData(void const* data, uint32_t size, uint8_t* compressedData, uint32_t& compressedSize) = 0;
	virtual bool DecompressData(uint8_t const* data, uint32_t size, void* decompressedData, uint32_t& decompressedSize) = 0;
	virtual uint32_t GetMaxLengthRequiredForCompressedData(uint32_t size) = 0;
	virtual uint32_t GetLengthOfCompressedData(uint8_t const* data) = 0;
	virtual uint32_t GetLengthOfDecompressedData(uint8_t const* data) = 0;
	virtual void GetCompressionVersionString(cIGZString& str) = 0;
};