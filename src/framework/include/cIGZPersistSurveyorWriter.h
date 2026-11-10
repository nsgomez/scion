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
#include "cIGZPersistWriter.h"

static const GZIID GZIID_cIGZPersistSurveyorWriter = 0x9B194C66;

class cIGZPersistSurveyorWriter : public cIGZPersistWriter
{
public:
	virtual void Clear() = 0;
	virtual cIGZPersistResource2* Resource() const = 0;

	virtual uint32_t NumObjectRefs() const = 0;
	virtual cIGZPersistSerializableReferent* ObjectRef(int32_t index) const = 0;
	virtual uint32_t IndexOfObjectRef(cIGZPersistSerializableReferent const* obj) const = 0;

	virtual uint32_t NumDependentResources() const = 0;
	virtual cIGZPersistResource2* DependentResource(int32_t index) const = 0;
	virtual uint32_t IndexOfDependentResource(cIGZPersistResource2 const* resource) const = 0;

	virtual void AddObjectRef(cIGZPersistSerializableReferent const* obj) = 0;
	virtual void AddDepResource(cIGZPersistResource2* resource) = 0;
};