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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <fstream>
#include "cRZTextOStream.h"

class cRZOFStream : public cRZTextOStream
{
public:
	cRZOFStream(cIGZString const& path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append);
	cRZOFStream(char const* path, bool binary, bool useLittleEndian, bool alwaysFlushStrings, bool append);
	virtual ~cRZOFStream();

public:
	virtual void Flush(void);

	virtual bool SetSint8(int8_t value);
	virtual bool SetUint8(uint8_t value);
	virtual bool SetSint16(int16_t value);
	virtual bool SetUint16(uint16_t value);
	virtual bool SetSint32(int32_t value);
	virtual bool SetUint32(uint32_t value);
	virtual bool SetSint64(int64_t value);
	virtual bool SetUint64(uint64_t value);
	virtual bool SetFloat32(float value);
	virtual bool SetFloat64(double value);
	virtual bool SetRZCharStr(char const* str);

	virtual bool SetVoid(void const* buffer, uint32_t size);
	virtual bool GetError(void) const;

protected:
	std::ofstream* outFile;
	bool binary;
	bool alwaysFlushStrings;
};