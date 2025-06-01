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

static const GZIID GZIID_cIGZDBSerialRecordAsIGZFile = 0x8A32C0AC;

class cIGZFile;
class cIGZPersistDBSerialRecord;

class cIGZDBSerialRecordAsIGZFile : public cIGZUnknown
{
public:
	virtual bool SetDBSerialRecord(cIGZPersistDBSerialRecord* record) = 0;
	virtual cIGZPersistDBSerialRecord* GetDBSerialRecord(void) = 0;
	virtual cIGZFile* AsIGZFile(void) = 0;
};