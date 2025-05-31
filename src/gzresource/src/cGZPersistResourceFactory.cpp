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

#include "cGZPersistResourceFactory.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cIGZPersistResource.h"

cGZPersistResourceFactory::cGZPersistResourceFactory(void) :
	refCount(0),
	criticalSection()
{
}

bool cGZPersistResourceFactory::CreateInstance(uint32_t type, uint32_t iid, void** outPtr, uint32_t n3, cIGZUnknown* ptr2)
{
	return CreateInstance(type, iid, outPtr);
}

bool cGZPersistResourceFactory::CreateInstance(cIGZPersistDBRecord& record, uint32_t iid, void** outPtr, int32_t n2, cIGZUnknown* ptr2)
{
	cRZCriticalSectionHolder lock(criticalSection);
	cIGZPersistResource* resource;

	bool haveInstance;
	if (n2 == 0 && ptr2 == NULL)
	{
		haveInstance = CreateInstance(record.GetKey().type, GZIID_cIGZPersistResource, reinterpret_cast<void**>(&resource));
	}
	else
	{
		haveInstance = CreateInstance(record.GetKey().type, GZIID_cIGZPersistResource, reinterpret_cast<void**>(&resource), n2, ptr2);
	}

	if (haveInstance)
	{
		resource->SetKey(record.GetKey());
		uint32_t type = resource->GetType();
		if (type != 0 && resource->QueryInterface(iid, outPtr))
		{
			return true;
		}

		resource->Release();
	}

	return false;
}

bool cGZPersistResourceFactory::Read(cIGZPersistResource& resource, cIGZPersistDBRecord& record)
{
	cRZCriticalSectionHolder lock(criticalSection);
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
	cRZCriticalSectionHolder lock(criticalSection);
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