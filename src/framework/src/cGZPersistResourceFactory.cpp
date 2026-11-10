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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGZPersistResourceFactory.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cIGZPersistResource.h"

cGZPersistResourceFactory::cGZPersistResourceFactory(void) :
	mRefCount(0),
	mCriticalSection()
{
}

bool cGZPersistResourceFactory::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZPersistResourceFactory:
		*outPtr = static_cast<cIGZPersistResourceFactory*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZPersistResourceFactory::AddRef()
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	return ++mRefCount;
}

uint32_t cGZPersistResourceFactory::Release()
{
	mCriticalSection.Lock();
	uint32_t result;

	if (mRefCount > 1)
	{
		result = --mRefCount;
		mCriticalSection.Unlock();
	}
	else
	{
		mCriticalSection.Unlock();
		delete this;
		result = 0;
	}

	return result;
}

bool cGZPersistResourceFactory::CreateInstance(uint32_t type, uint32_t iid, void** outPtr, uint32_t n3, cIGZUnknown* ptr2)
{
	return CreateInstance(type, iid, outPtr);
}

bool cGZPersistResourceFactory::CreateInstance(cIGZPersistDBRecord& record, uint32_t iid, void** outPtr, int32_t n2, cIGZUnknown* ptr2)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	cIGZPersistResource* resource;

	bool result;
	if (n2 == 0 && ptr2 == NULL)
	{
		result = CreateInstance(record.GetKey().type, GZIID_cIGZPersistResource, reinterpret_cast<void**>(&resource));
	}
	else
	{
		result = CreateInstance(record.GetKey().type, GZIID_cIGZPersistResource, reinterpret_cast<void**>(&resource), n2, ptr2);
	}

	if (result)
	{
		resource->SetKey(record.GetKey());
		result = Read(*resource, record) && resource->QueryInterface(iid, outPtr);
		resource->Release();
	}

	return result;
}

bool cGZPersistResourceFactory::Read(cIGZPersistResource& resource, cIGZPersistDBRecord& record)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	cIGZPersistDBSerialRecord* serialRecord;

	bool result = false;
	if (record.QueryInterface(GZIID_cIGZPersistDBSerialRecord, reinterpret_cast<void**>(&serialRecord)))
	{
		result = SerialRead(resource, *serialRecord);
		serialRecord->Release();
	}

	return result;
}

bool cGZPersistResourceFactory::Write(cIGZPersistResource const& resource, cIGZPersistDBRecord& record)
{
	cRZCriticalSectionHolder lock(mCriticalSection);
	cIGZPersistDBSerialRecord* serialRecord;

	bool result = false;
	if (record.QueryInterface(GZIID_cIGZPersistDBSerialRecord, reinterpret_cast<void**>(&serialRecord)))
	{
		result = SerialWrite(const_cast<cIGZPersistResource&>(resource), *serialRecord);
		serialRecord->Release();
	}

	return result;
}

bool cGZPersistResourceFactory::CreateInstance(cIGZPersistDBRecord& record, uint32_t iid, void** outPtr)
{
	return CreateInstance(record, iid, outPtr, 0, NULL);
}