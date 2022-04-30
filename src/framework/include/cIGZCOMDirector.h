/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZCOMDirector = 0xA21EE941;

class cIGZCOM;
class cIGZFramework;
class cIGZString;
class cRZCOMDllDirector;

class cIGZCOMDirector : public cIGZUnknown
{
public:
	typedef void (*ClassObjectEnumerationCallback)(GZGUID, uint32_t, void*);

	virtual bool InitializeCOM(cIGZCOM* pCOM, const cIGZString& libraryPath) = 0;
	virtual bool OnStart(cIGZCOM* pCOM) = 0;

	virtual void EnumClassObjects(ClassObjectEnumerationCallback callback, void* context) = 0;
	virtual bool GetClassObject(GZGUID clsid, GZIID iid, void** outPtr) = 0;

	virtual bool CanUnloadNow(void) = 0;
	virtual bool OnUnload(void) = 0;

	virtual uint32_t RefCount(void) = 0;
	virtual uint32_t RemoveRef(void) = 0;

	virtual cIGZFramework* FrameWork(void) = 0;
	virtual cIGZCOM* GZCOM(void) = 0;

	virtual GZGUID GetDirectorID() const = 0;
	virtual bool GetLibraryPath(cIGZString& path) = 0;
	virtual uint32_t GetHeapAllocatedSize(void) = 0;
};