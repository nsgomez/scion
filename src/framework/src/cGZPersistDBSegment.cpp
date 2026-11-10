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

#include "cGZPersistDBSegment.h"
#include "cIGZPersistDBRecord.h"

cGZPersistDBSegment::cGZPersistDBSegment(void) :
	refCount(0),
	criticalSection()
{
}

cGZPersistDBSegment::~cGZPersistDBSegment()
{
}

bool cGZPersistDBSegment::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistDBSegment)
	{
		*outPtr = static_cast<cIGZPersistDBSegment*>(this);
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
	}
	else
	{
		return false;
	}

	AddRef();
	return true;
}

uint32_t cGZPersistDBSegment::AddRef(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return ++refCount;
}

uint32_t cGZPersistDBSegment::Release(void)
{
	criticalSection.Lock();
	uint32_t retVal;

	if (refCount > 1)
	{
		retVal = --refCount;
		criticalSection.Unlock();
	}
	else
	{
		criticalSection.Unlock();
		FinalRelease();
		retVal = 0;
	}

	return retVal;
}

bool cGZPersistDBSegment::Lock(void)
{
	criticalSection.Lock();
	return true;
}

bool cGZPersistDBSegment::Unlock(void)
{
	criticalSection.Unlock();
	return true;
}

uint32_t cGZPersistDBSegment::GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter)
{
	return 0;
}

uint32_t cGZPersistDBSegment::GetResourceKeyList(cIGZPersistResourceKeyList& list)
{
	return GetResourceKeyList(&list, NULL) != 0;
}

bool cGZPersistDBSegment::TestForRecord(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoTestForRecord(key);
}

int32_t cGZPersistDBSegment::GetRecordSize(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoGetRecordSize(key);
}

bool cGZPersistDBSegment::OpenRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out, uint32_t flags)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoOpenRecord(key, out, flags);
}

bool cGZPersistDBSegment::CreateNewRecord(cGZPersistResourceKey const& key, cIGZPersistDBRecord** out)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoCreateNewRecord(key, out);
}

bool cGZPersistDBSegment::CloseRecord(cIGZPersistDBRecord* record)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoCloseRecord(record);
}

bool cGZPersistDBSegment::CloseRecord(cIGZPersistDBRecord** recordPtr)
{
	cIGZPersistDBRecord* record = *recordPtr;
	if (CloseRecord(record))
	{
		record->Release();
		return true;
	}

	return false;
}

bool cGZPersistDBSegment::AbortRecord(cIGZPersistDBRecord* record)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoAbortRecord(record);
}

bool cGZPersistDBSegment::AbortRecord(cIGZPersistDBRecord** recordPtr)
{
	cIGZPersistDBRecord* record = *recordPtr;
	if (AbortRecord(record))
	{
		record->Release();
		return true;
	}

	return false;
}

bool cGZPersistDBSegment::DeleteRecord(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoDeleteRecord(key);
}

uint32_t cGZPersistDBSegment::ReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoReadRecord(key, data, length);
}

bool cGZPersistDBSegment::WriteRecord(cGZPersistResourceKey const& key, void* data, uint32_t length)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoWriteRecord(key, data, length);
}

void cGZPersistDBSegment::FinalRelease(void)
{
	delete this;
}