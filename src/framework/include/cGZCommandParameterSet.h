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

#pragma once
#include <map>
#include "cIGZCommandParameterSet.h"
#include "cRZVariant.h"

class cGZCommandParameterSet : public cIGZCommandParameterSet
{
public:
	cGZCommandParameterSet();
	virtual ~cGZCommandParameterSet();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual uint32_t GetParameterCount() const;
	virtual void SetParameterCount(uint32_t count);

	virtual cIGZVariant* operator[](GZGUID id);
	virtual cIGZVariant* GetParameter(GZGUID id);
	virtual void SetParameter(GZGUID id, cIGZVariant const& param);
	virtual void AppendParameter(cIGZVariant const& param);
	virtual void RemoveParameter(GZGUID id);

	virtual void SetStatusParameterValue(int32_t status);
	virtual bool CompareParameterString(GZGUID id, char const* other, bool caseSensitive);
	virtual bool ValidateParameter(GZGUID id, uint32_t type);

	virtual uint32_t GetFirstParameterID(void) const;
	virtual uint32_t GetNextParameterID(GZGUID previousId) const;

protected:
	uint32_t mRefCount;
	std::map<GZGUID, cRZVariant> mParamMap;
	cRZVariant mFallback;
};