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

#include "cGZMessage2CommandDispatcher.h"
#include "cGZMessageServer2.h"
#include "cIGZVariant.h"
#include "cRZSysServPtr.h"

cGZMessage2CommandDispatcher::cGZMessage2CommandDispatcher() :
	mRefCount(0)
{
}

cGZMessage2CommandDispatcher::~cGZMessage2CommandDispatcher() { }

bool cGZMessage2CommandDispatcher::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZCommandDispatcher:
		*outPtr = static_cast<cIGZCommandDispatcher*>(this);
		break;

	case GZIID_cIGZCommandDispatcherUnknown:
		*outPtr = static_cast<cIGZCommandDispatcher*>(this); // TODO
		break;

	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		break;

	default:
		return false;
	}

	AddRef();
	return true;
}

uint32_t cGZMessage2CommandDispatcher::AddRef()
{
	return ++mRefCount;
}

uint32_t cGZMessage2CommandDispatcher::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cGZMessage2CommandDispatcher::Init()
{
	if (mMessageServer == NULL)
	{
		mMessageServer = cRZSysServPtr<cIGZMessageServer2, GZIID_cIGZMessageServer2, RZSRVID_cGZMessageServer2>();
	}

	if (mMessage == NULL)
	{
		mMessage = new cGZMessage2CommandDispatcherMessage();
	}

	if (mInputParams == NULL)
	{
		mInputParams = new cGZCommandParameterSet();
	}

	if (mOutputParams == NULL)
	{
		mOutputParams = new cGZCommandParameterSet();
	}

	if (mGenerator == NULL)
	{
		mGenerator = new cGZStringCommandGenerator();
		mGenerator->Init();
	}

	return true;
}

bool cGZMessage2CommandDispatcher::Shutdown()
{
	mMessageServer = NULL;
	mMessage = NULL;
	mInputParams = NULL;
	mOutputParams = NULL;

	if (mGenerator != NULL)
	{
		mGenerator->Shutdown();
		mGenerator = NULL;
	}

	return true;
}

int32_t cGZMessage2CommandDispatcher::ExecuteCommand(uint32_t commandId, cIGZCommandParameterSet* inputParams, cIGZCommandParameterSet* outputParams)
{
	int32_t result = kGZCommandDispatcherResult_NoMessageServer;

	if (mMessageServer != NULL)
	{
		if (inputParams == NULL)
		{
			mInputParams->SetParameterCount(0);
			inputParams = mInputParams;
		}

		if (outputParams == NULL)
		{
			mOutputParams->SetParameterCount(0);
			outputParams = mOutputParams;
		}

		mMessage->SetType(commandId);

		mMessage->SetVoid1(inputParams);
		mMessage->SetInputParams(inputParams);

		mMessage->SetVoid2(outputParams);
		mMessage->SetOutputParams(outputParams);

		mMessageServer->MessageSend(mMessage);
		result = kGZCommandDispatcherResult_Success;

		cIGZVariant* status = mOutputParams->GetParameter(kGZCommandStatusParameterID);
		if (status != NULL)
		{
			result = status->GetValSint32();
		}
		else
		{
			outputParams->SetStatusParameterValue(result);
		}
	}

	return result;
}