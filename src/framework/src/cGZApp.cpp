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

#include "cGZApp.h"
#include "cIGZFramework.h"
#include "RZStatics.h"

cGZApp::cGZApp(char const* moduleName) :
	cRZSystemService(RZSRVID_cGZApp, -2000000),
	moduleName(moduleName)
{
}

cGZApp::~cGZApp()
{
}

bool cGZApp::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZApp)
	{
		*outPtr = static_cast<cIGZApp*>(this);
		AddRef();
		return true;
	}
	else
	{
		return cRZSystemService::QueryInterface(iid, outPtr);
	}
}

uint32_t cGZApp::AddRef(void)
{
	return cRZSystemService::AddRef();
}

uint32_t cGZApp::Release(void)
{
	return cRZSystemService::Release();
}

cIGZSystemService* cGZApp::AsIGZSystemService(void)
{
	return this;
}

char const* cGZApp::ModuleName(void)
{
	return moduleName.c_str();
}

cIGZFramework* cGZApp::Framework(void)
{
	return RZGetFramework();
}

bool cGZApp::AddApplicationService(cIGZSystemService* service)
{
	return Framework()->AddSystemService(service);
}

bool cGZApp::PreFrameworkInit(void)
{
	return true;
}

bool cGZApp::PostFrameworkInit(void)
{
	return true;
}

bool cGZApp::PreFrameworkShutdown(void)
{
	return true;
}

bool cGZApp::GZRun(void)
{
	return false;
}

bool cGZApp::LoadRegistry(void)
{
	return true;
}

void cGZApp::AddDynamicLibrariesHere(void)
{
}

void cGZApp::AddCOMDirectorsHere(void)
{
}

void cGZApp::AddApplicationServicesHere(void)
{
}