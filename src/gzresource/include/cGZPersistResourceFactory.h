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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include "cIGZPersistResourceFactory.h"
#include "cRZCriticalSection.h"

class cIGZPersistDBSerialRecord;

class cGZPersistResourceFactory : public cIGZPersistResourceFactory
{
public:
	cGZPersistResourceFactory(void);
	virtual ~cGZPersistResourceFactory(void) { }

public:
	virtual bool CreateInstance(uint32_t type, uint32_t iid, void** outPtr, uint32_t, cIGZUnknown*);
	virtual bool CreateInstance(cIGZPersistDBRecord& record, uint32_t iid, void** outPtr, int32_t, cIGZUnknown*);

	virtual bool Read(cIGZPersistResource& resource, cIGZPersistDBRecord& record);
	virtual bool Write(cIGZPersistResource const& resource, cIGZPersistDBRecord& record);

protected:
	virtual bool CreateInstance(uint32_t type, uint32_t iid, void** outPtr) = 0;
	virtual bool CreateInstance(cIGZPersistDBRecord& record, uint32_t iid, void** outPtr);
	virtual bool SerialRead(cIGZPersistResource& resource, cIGZPersistDBSerialRecord& record) = 0;
	virtual bool SerialWrite(cIGZPersistResource& resource, cIGZPersistDBSerialRecord& record) = 0;

protected:
	uint32_t refCount;
	cRZCriticalSection criticalSection;
};