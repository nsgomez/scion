/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include "cGZCommandParameterSet.h"
#include "cRZString.h"

cGZCommandParameterSet::cGZCommandParameterSet() :
	mRefCount(0)
{
}

cGZCommandParameterSet::~cGZCommandParameterSet()
{
}

bool cGZCommandParameterSet::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZCommandParameterSet:
		*outPtr = static_cast<cIGZCommandParameterSet*>(this);
		AddRef();
		return true;

	case GZIID_cIGZCommandParameterSet2:
		*outPtr = static_cast<cIGZCommandParameterSet*>(this);
		AddRef();
		return true;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	default:
		return false;
	}
}

uint32_t cGZCommandParameterSet::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZCommandParameterSet::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

uint32_t cGZCommandParameterSet::GetParameterCount() const
{
	return mParamMap.size();
}

void cGZCommandParameterSet::SetParameterCount(uint32_t count)
{
	while (mParamMap.size() < count)
	{
		AppendParameter(cRZVariant());
	}

	while (mParamMap.size() > count)
	{
		mParamMap.erase(--mParamMap.end());
	}
}

cIGZVariant* cGZCommandParameterSet::operator[](GZGUID id)
{
	std::map<GZGUID, cRZVariant>::iterator it = mParamMap.find(id);
	if (it != mParamMap.end())
	{
		return &it->second;
	}

	return &mFallback;
}

cIGZVariant* cGZCommandParameterSet::GetParameter(GZGUID id)
{
	std::map<GZGUID, cRZVariant>::iterator it = mParamMap.find(id);
	if (it != mParamMap.end())
	{
		return &it->second;
	}

	return &mFallback;
}

void cGZCommandParameterSet::SetParameter(GZGUID id, cIGZVariant const& param)
{
	mParamMap[id] = param;
}

void cGZCommandParameterSet::AppendParameter(cIGZVariant const& param)
{
	GZGUID id = 0;
	if (mParamMap.size() != 0)
	{
		id = (--mParamMap.end())->first + 1;
	}

	SetParameter(id, param);
}

void cGZCommandParameterSet::RemoveParameter(GZGUID id)
{
	std::map<GZGUID, cRZVariant>::iterator it = mParamMap.find(id);
	if (it != mParamMap.end())
	{
		mParamMap.erase(it);
	}
}

void cGZCommandParameterSet::SetStatusParameterValue(int32_t status)
{
	SetParameter(kGZCommandStatusParameterID, cRZVariant(status));
}

bool cGZCommandParameterSet::CompareParameterString(GZGUID id, char const* other, bool caseSensitive)
{
	if (id == kGZCommandAnyParameterID)
	{
		std::map<GZGUID, cRZVariant>::iterator it = mParamMap.begin();
		std::map<GZGUID, cRZVariant>::iterator end = mParamMap.end();

		while (it != end)
		{
			GZGUID paramId = it->first;
			if (CompareParameterString(paramId, other, caseSensitive))
			{
				return true;
			}

			++it;
		}

		return false;
	}

	cIGZVariant* paramFromMap = GetParameter(id);
	if (paramFromMap == NULL)
	{
		return false;
	}

	cRZString str;
	if (paramFromMap->GetValString(str))
	{
		return str.Find(other, 0, caseSensitive) != -1
			&& str.Strlen() == strlen(other);
	}

	return false;
}

bool cGZCommandParameterSet::ValidateParameter(GZGUID id, uint32_t type)
{
	if (GetParameter(id) != NULL)
	{
		if (type == 0xFF)
		{
			return true;
		}
	}

	return false;
}

uint32_t cGZCommandParameterSet::GetFirstParameterID() const
{
	if (mParamMap.size() != 0)
	{
		return mParamMap.begin()->first;
	}

	return -1;
}

uint32_t cGZCommandParameterSet::GetNextParameterID(GZGUID previousId) const
{
	std::map<GZGUID, cRZVariant>::const_iterator it = mParamMap.find(previousId);
	std::map<GZGUID, cRZVariant>::const_iterator end = mParamMap.end();

	if (it == end)
	{
		return -1;
	}

	++it;

	if (it != end)
	{
		return it->first;
	}

	return -1;
}