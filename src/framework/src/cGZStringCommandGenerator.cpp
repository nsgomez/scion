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

#include "cGZStringCommandGenerator.h"
#include "cRZSysServPtr.h"

cGZStringCommandGenerator::cGZStringCommandGenerator()
{
}

cGZStringCommandGenerator::~cGZStringCommandGenerator()
{
}

bool cGZStringCommandGenerator::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZCommandGenerator:
		*outPtr = static_cast<cIGZCommandGenerator*>(this);
		AddRef();
		return true;

	case GZIID_cIGZCommandGenerator2:
		*outPtr = static_cast<cIGZCommandGenerator*>(this);
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

uint32_t cGZStringCommandGenerator::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZStringCommandGenerator::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cGZStringCommandGenerator::Init()
{
	if (mCommandSrv == NULL)
	{
		mCommandSrv = cRZSysServPtr<cIGZCommandServer, GZIID_cIGZCommandServer, RZSRVID_cGZCommandServer>();
	}

	return true;
}

bool cGZStringCommandGenerator::Shutdown()
{
	mCommandSrv = NULL;
	return true;
}

bool cGZStringCommandGenerator::CommandRegistered(uint32_t commandId, cIGZString const& commandName, cIGZString const& commandInfo)
{
	return true;
}

bool cGZStringCommandGenerator::CommandUnregistered(uint32_t commandId, cIGZString const& commandName, cIGZString const& commandInfo)
{
	return true;
}

uint32_t cGZStringCommandGenerator::ExecuteCommand(char const* str, uint32_t length, cIGZCommandParameterSet* params)
{
	// TODO
	return 0;
}

bool cGZStringCommandGenerator::ConvertStringToCommand(char const* str, uint32_t length, uint32_t& commandIdOut, cIGZCommandParameterSet*& paramsOut)
{
	// TODO
	return false;
}

bool cGZStringCommandGenerator::ConvertCommandToString(uint32_t commandId, cIGZCommandParameterSet* params, cIGZString& strOut)
{
	// TODO
	return false;
}

bool cGZStringCommandGenerator::ConvertCommandOutputToString(cIGZCommandParameterSet* params, cIGZString& strOut)
{
	// TODO
	return false;
}