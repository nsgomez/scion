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

#include "cGZPersistDBSegment.h"
#include "cIGZPersistDBRecord.h"

cGZPersistDBSegment::cGZPersistDBSegment(void) :
	refCount(0),
	criticalSection()
{
}

bool cGZPersistDBSegment::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZPersistDBSegment)
	{
		*outPtr = static_cast<cIGZPersistDBSegment*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cGZPersistDBSegment::AddRef(void)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return ++refCount;
}

uint32_t cGZPersistDBSegment::Release(void)
{
	criticalSection.Lock();
	if (refCount < 2)
	{
		criticalSection.Unlock();
		delete this;
		return 0;
	}
	else
	{
		uint32_t retVal = --refCount;
		criticalSection.Unlock();
		return retVal;
	}
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

bool cGZPersistDBSegment::GetResourceKeyList(cIGZPersistResourceKeyList* list, cIGZPersistResourceKeyFilter* filter)
{
	return false;
}

bool cGZPersistDBSegment::GetResourceKeyList(cIGZPersistResourceKeyList& list)
{
	return GetResourceKeyList(&list, NULL);
}

bool cGZPersistDBSegment::TestForRecord(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return DoTestForRecord(key);
}

int32_t cGZPersistDBSegment::GetRecordSize(cGZPersistResourceKey const& key)
{
	cRZCriticalSectionHolder lock(criticalSection);
	return GetRecordSize(key);
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

bool cGZPersistDBSegment::ReadRecord(cGZPersistResourceKey const& key, void* data, uint32_t& length)
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