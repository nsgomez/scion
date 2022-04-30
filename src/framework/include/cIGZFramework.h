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

static const GZIID GZIID_cIGZFramework = 0x00000065;

class cIGZApp;
class cIGZCOM;
class cIGZDebugStream;
class cIGZFrameworkHooks;
class cIGZIStream;
class cIGZOStream;
class cIGZSystemService;
class cIGZUnknownEnumerator;
class cRZCmdLine;
class cRZExceptionNotification;

enum FrameworkState
{
	GZFrameworkState_InitializingCOM,
	GZFrameworkState_PreFrameworkInit,
	GZFrameworkState_UnknownState2,
	GZFrameworkState_PreAppInit,
	GZFrameworkState_InitializingApp,
	GZFrameworkState_PostAppInit,
	GZFrameworkState_PostFrameworkInit,
	GZFrameworkState_Running,
};

class cIGZFramework : public cIGZUnknown
{
public:
	virtual bool AddSystemService(cIGZSystemService* service) = 0;
	virtual bool RemoveSystemService(cIGZSystemService* service) = 0;
	virtual bool GetSystemService(GZGUID serviceId, GZIID iid, void** outPtr) = 0;
	virtual bool EnumSystemServices(cIGZUnknownEnumerator* enumerator, cIGZUnknown* context, GZIID iid) = 0;

	virtual bool AddHook(cIGZFrameworkHooks* hook) = 0;
	virtual bool RemoveHook(cIGZFrameworkHooks* hook) = 0;

	virtual bool AddToTick(cIGZSystemService* service) = 0;
	virtual bool RemoveFromTick(cIGZSystemService* service) = 0;

	virtual bool AddToOnIdle(cIGZSystemService* service) = 0;
	virtual bool RemoveFromOnIdle(cIGZSystemService* service) = 0;

	virtual int32_t GetOnIdleInterval(void) const = 0;
	virtual void SetOnIdleInterval(int32_t idleFrameInterval) = 0;

	virtual void OnTick(uint32_t totalTickFrames) = 0;
	virtual void OnIdle(void) = 0;

	virtual bool IsTickEnabled(void) const = 0;
	virtual void ToggleTick(bool toggle) = 0;

	virtual void Quit(int exitCode) = 0;
	virtual void AbortiveQuit(int exitCode) = 0;

	virtual cRZCmdLine* CommandLine(void) = 0;

	virtual bool IsInstall(void) const = 0;

	virtual cIGZCOM* GetCOMObject(void) = 0;
	virtual FrameworkState GetState(void) const = 0;

	virtual cIGZDebugStream* GetDebugStream(void) = 0;
	virtual cIGZDebugStream* DefaultDebugStream(void) = 0;
	virtual cIGZDebugStream* DebugStream(void) = 0;
	virtual void SetDebugStream(cIGZDebugStream* debugStream) = 0;

	virtual void SetDebugLevel(int32_t level) = 0;
	virtual int GetDebugLevel(void) const = 0;

	virtual cIGZOStream* StdOut(void) = 0;
	virtual cIGZOStream* StdErr(void) = 0;
	virtual cIGZIStream* StdIn(void) = 0;

	virtual bool GetStream(int32_t streamNum, GZIID iid, void** outPtr) = 0;
	virtual bool SetStream(int32_t streamNum, cIGZUnknown* stream) = 0;

	virtual void SetApplication(cIGZApp* app) = 0;
	virtual cIGZApp* Application(void) const = 0;

	virtual void ReportException(char const* exceptionReport) = 0;
	virtual cRZExceptionNotification* ExceptionNotificationObj(void) const = 0;
};