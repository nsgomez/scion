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
#include "cIGZCommandGenerator.h"
#include "cIGZCommandServer.h"
#include "cRZAutoRefCount.h"

class cGZStringCommandGenerator : public cIGZCommandGenerator
{
public:
	cGZStringCommandGenerator();
	virtual ~cGZStringCommandGenerator();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

	virtual bool Init();
	virtual bool Shutdown();

	virtual bool CommandRegistered(uint32_t commandId, cIGZString const& commandName, cIGZString const& commandInfo);
	virtual bool CommandUnregistered(uint32_t commandId, cIGZString const& commandName, cIGZString const& commandInfo);

	virtual uint32_t ExecuteCommand(char const* str, uint32_t length, cIGZCommandParameterSet* params);
	virtual bool ConvertStringToCommand(char const* str, uint32_t length, uint32_t& commandIdOut, cIGZCommandParameterSet*& paramsOut);
	virtual bool ConvertCommandToString(uint32_t commandId, cIGZCommandParameterSet* params, cIGZString& strOut);
	virtual bool ConvertCommandOutputToString(cIGZCommandParameterSet* params, cIGZString& strOut);

protected:
	uint32_t mRefCount;
	cRZAutoRefCount<cIGZCommandServer> mCommandSrv;
};