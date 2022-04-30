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
#include <string>
#include "cIGZApp.h"
#include "cRZSystemService.h"

static const GZGUID RZSRVID_cGZApp = 0x00000066;

class cGZApp : public cRZSystemService, public cIGZApp
{
public:
	cGZApp(char const* moduleName);
	virtual ~cGZApp();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual cIGZSystemService* AsIGZSystemService();
	virtual char const* ModuleName();
	virtual cIGZFramework* Framework();

	virtual bool AddApplicationService(cIGZSystemService* service);

	virtual bool PreFrameworkInit();
	virtual bool PostFrameworkInit();
	virtual bool PreFrameworkShutdown();

	virtual bool GZRun();
	
	virtual bool LoadRegistry();

	virtual void AddDynamicLibrariesHere();
	virtual void AddCOMDirectorsHere();
	virtual void AddApplicationServicesHere();

protected:
	std::string moduleName;
};