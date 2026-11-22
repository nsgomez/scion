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
#include "cGZCommandParameterSet.h"
#include "cGZMessage2CommandDispatcherMessage.h"
#include "cGZStringCommandGenerator.h"
#include "cIGZCommandDispatcher.h"
#include "cIGZMessageServer2.h"
#include "cRZAutoRefCount.h"

enum GZCommandDispatcherResult
{
	kGZCommandDispatcherResult_Success = 0,
	// TODO: ???
	kGZCommandDispatcherResult_NoMessageServer = 2,
};

class cGZMessage2CommandDispatcher : public cIGZCommandDispatcher
{
public:
	cGZMessage2CommandDispatcher();
	virtual ~cGZMessage2CommandDispatcher();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

	virtual bool Init();
	virtual bool Shutdown();

	virtual int32_t ExecuteCommand(uint32_t commandId, cIGZCommandParameterSet* inputParams, cIGZCommandParameterSet* outputParams);

protected:
	uint32_t mRefCount;
	cRZAutoRefCount<cIGZMessageServer2> mMessageServer;
	cRZAutoRefCount<cGZMessage2CommandDispatcherMessage> mMessage;
	cRZAutoRefCount<cGZCommandParameterSet> mInputParams;
	cRZAutoRefCount<cGZCommandParameterSet> mOutputParams;
	cRZAutoRefCount<cGZStringCommandGenerator> mGenerator;
};