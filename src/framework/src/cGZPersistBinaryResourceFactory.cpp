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

#include "cGZPersistBinaryResource.h"
#include "cGZPersistBinaryResourceFactory.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cRZAutoRefCount.h"

cGZPersistBinaryResourceFactory::cGZPersistBinaryResourceFactory() :
	cGZPersistResourceFactory()
{
}

bool cGZPersistBinaryResourceFactory::CreateInstance(uint32_t type, uint32_t iid, void** outPtr)
{
	if (type == GZTYPE_cGZPersistBinaryResource)
	{
		cRZAutoRefCount<cGZPersistBinaryResource> resource(new cGZPersistBinaryResource());
		return resource->QueryInterface(iid, outPtr);
	}

	return false;
}

bool cGZPersistBinaryResourceFactory::SerialRead(cIGZPersistResource& resource, cIGZPersistDBSerialRecord& record)
{
	bool result = false;

	if (resource.GetType() == GZTYPE_cGZPersistBinaryResource)
	{
		cIGZPersistBinaryResource* binaryResource = NULL;
		if (resource.QueryInterface(GZIID_cIGZPersistBinaryResource, reinterpret_cast<void**>(&binaryResource)))
		{
			uint32_t size = record.GetSize();
			if (size > 0)
			{
				uint8_t* buffer = new uint8_t[size];
				if (record.GetFieldVoid(buffer, size) && binaryResource->SetData(buffer, size))
				{
					result = true;
				}
				else
				{
					result = false;
				}

				delete[] buffer;
			}
			else if (size == 0)
			{
				result = binaryResource->SetData(NULL, 0);
			}
		}

		if (binaryResource != NULL)
		{
			binaryResource->Release();
		}
	}

	return result;
}

bool cGZPersistBinaryResourceFactory::SerialWrite(cIGZPersistResource& resource, cIGZPersistDBSerialRecord& record)
{
	bool result = false;

	if (resource.GetType() == GZTYPE_cGZPersistBinaryResource)
	{
		cIGZPersistBinaryResource* binaryResource = NULL;
		if (resource.QueryInterface(GZIID_cIGZPersistBinaryResource, reinterpret_cast<void**>(&binaryResource)))
		{
			void* data = binaryResource->GetData();
			uint32_t length = binaryResource->GetDataLength();

			if (data != NULL && length > 0)
			{
				result = record.SetFieldVoid(binaryResource->GetData(), length);
			}
			else if (data == NULL || length == 0)
			{
				if (binaryResource != NULL)
				{
					binaryResource->Release();
				}

				return true;
			}
		}

		if (binaryResource != NULL)
		{
			binaryResource->Release();
		}
	}

	return result;
}