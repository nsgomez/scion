/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
#include "cRZCOMDllDirector.h"

static const GZGUID kGZResManCOMDirectorID = 0xC3CAEC3B;

class cGZResManCOMDirector : public cRZCOMDllDirector
{
public:
	cGZResManCOMDirector(void);
	virtual ~cGZResManCOMDirector(void) { }

public:
	virtual bool OnStart(cIGZCOM* com);
	virtual GZGUID GetDirectorID(void) const;

protected:
	static cIGZUnknown* CreateGZDBSegmentDirectoryFiles(void);
	static cIGZUnknown* CreateGZDBSegmentMultiPackedFiles(void);
	static cIGZUnknown* CreateGZDBSegmentPackedFile(void);
	static cIGZUnknown* CreateGZDBSerialRecordAsIGZFile(void);
	static cIGZUnknown* CreateASCIIWriter(void);
	static cIGZUnknown* CreateBinaryReader(void);
	static cIGZUnknown* CreateBinaryResourceFactory(void);
	static cIGZUnknown* CreateBinaryWriter(void);
	static cIGZUnknown* CreateSurveyorWriter(void);
	static cIGZUnknown* CreateGZResourceKeyList(void);
	static cIGZUnknown* CreateGZResourceManager(void);
	static cIGZUnknown* CreateGZStringResourceFactory(void);
	static cIGZUnknown* CreatecGZWinKeyAcceleratorResFactory(void);
	static cIGZUnknown* CreatecRZFastCompression3(void);
	static cIGZUnknown* CreatecSLTokenResourceFactory(void);
};