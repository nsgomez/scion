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
#include "cIGZCommandDispatcherObject.h"
#include "cIGZCommandParameterSet.h"
#include "cRZAutoRefCount.h"
#include "cRZMessage2Standard.h"

static const GZIID GZIID_cGZMessage2CommandDispatcherMessage = 0x4B99446A;

class cGZMessage2CommandDispatcherMessage : public cIGZCommandDispatcherObject, public cRZMessage2Standard
{
public:
	cGZMessage2CommandDispatcherMessage();
	virtual ~cGZMessage2CommandDispatcherMessage();

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

	void SetInputParams(cIGZCommandParameterSet* params) { mInputParams = params; }
	void SetOutputParams(cIGZCommandParameterSet* params) { mOutputParams = params; }

protected:
	cRZAutoRefCount<cIGZCommandParameterSet> mInputParams;
	cRZAutoRefCount<cIGZCommandParameterSet> mOutputParams;
};