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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZCommandParameterSet = 0x4B8C39E4;
static const GZIID GZIID_cIGZCommandParameterSet2 = 0x4B8C39E5; // TODO: is there a better name for this?

static const GZGUID kGZCommandStatusParameterID = 0x2B999D2B;
static const GZGUID kGZCommandAnyParameterID = 0xFFFFFFFE;

class cIGZVariant;

class cIGZCommandParameterSet : public cIGZUnknown
{
public:
	virtual uint32_t GetParameterCount() const = 0;
	virtual void SetParameterCount(uint32_t count) = 0;

	virtual cIGZVariant* operator[](GZGUID id) = 0;
	virtual cIGZVariant* GetParameter(GZGUID id) = 0;
	virtual void SetParameter(GZGUID id, cIGZVariant const& param) = 0;
	virtual void AppendParameter(cIGZVariant const& param) = 0;
	virtual void RemoveParameter(GZGUID id) = 0;

	virtual void SetStatusParameterValue(int32_t status) = 0;
	virtual bool CompareParameterString(GZGUID id, char const* other, bool caseSensitive) = 0;
	virtual bool ValidateParameter(GZGUID id, uint32_t type) = 0;

	virtual uint32_t GetFirstParameterID(void) const = 0;
	virtual uint32_t GetNextParameterID(GZGUID previousId) const = 0;
};