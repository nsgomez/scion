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

#include <vector>
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZCommandServer = 0xCB90253A;
static const GZIID GZIID_cIGZCommandServer2 = 0xCB90253B; // TODO: naming?
static const GZGUID RZSRVID_cGZCommandServer = 0xEB903A32; // TODO: move later

class cIGZCommandDispatcher;
class cIGZCommandGenerator;
class cIGZCommandParameterSet;
class cIGZString;

class cIGZCommandServer : public cIGZUnknown
{
public:
	struct cGZCommandInfo
	{
		// TODO
	};

public:
	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;

public:
	virtual void CreateCommandParameterSet(cIGZCommandParameterSet** paramSet) = 0;

	virtual bool RegisterCommand(GZGUID id, char const* name, char const* paramInfo) = 0;
	virtual bool RegisterCommands(cGZCommandInfo* info, uint32_t count);
	virtual bool RegisterCommandParameterInfo(GZGUID id, char const* paramInfo) = 0;
	virtual bool UnregisterCommand(GZGUID id) = 0;
	virtual bool UnregisterCommands(cGZCommandInfo* info, uint32_t count);

	virtual bool GetCommandInfo(GZGUID id, cIGZString& name, cIGZString* paramInfo) = 0;
	virtual GZGUID GetCommandID(cIGZString const& name) const = 0;
	virtual GZGUID GetCommandIDFromCommandString(char const* str, uint32_t length) = 0;

	virtual void EnumerateCommands(std::vector<GZGUID>& commandIds) const = 0;

	virtual bool RegisterCommandGenerator(cIGZCommandGenerator* generator) = 0;
	virtual bool UnregisterCommandGenerator(cIGZCommandGenerator* generator) = 0;

	virtual cIGZCommandDispatcher const* GetCommandDispatcher() const = 0;
	virtual void SetCommandDispatcher(cIGZCommandDispatcher* dispatcher) = 0;

	virtual uint32_t ExecuteCommand(char const* str, uint32_t length, cIGZCommandParameterSet* paramsOut) = 0;
	virtual uint32_t ExecuteCommand(GZGUID id, cIGZCommandParameterSet* paramsIn, cIGZCommandParameterSet* paramsOut) = 0;
	virtual uint32_t ExecuteCommandFormatted(char const* formatStr, cIGZCommandParameterSet* paramsOut, ...) = 0;

	virtual bool ConvertStringToCommand(char const* str, uint32_t length, uint32_t& id, cIGZCommandParameterSet*& paramSet) = 0;
	virtual bool ConvertCommandToString(uint32_t id, cIGZCommandParameterSet* params, cIGZString& str) = 0;
	virtual bool ConvertCommandOutputToString(cIGZCommandParameterSet* params, cIGZString& str) = 0;
};