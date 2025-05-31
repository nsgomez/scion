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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "cGZDBSegmentDirectoryFiles.h"
#include "cGZDBSegmentPackedFile.h"
#include "cGZDBSerialRecordAsIGZFile.h"
#include "cGZPersistASCIIWriter.h"
#include "cGZPersistBinaryReader.h"
#include "cGZPersistBinaryResourceFactory.h"
#include "cGZPersistBinaryWriter.h"
#include "cGZPersistResourceKeyList.h"
#include "cGZPersistResourceManager.h"
#include "cGZPersistSurveyorWriter.h"
#include "cIGZCOM.h"
#include "cIGZFramework.h"
#include "cIGZSystemService.h"
#include "cRZFastCompression3.h"
#include "GZResourceD.h"

cGZResManCOMDirector::cGZResManCOMDirector(void) :
	cRZCOMDllDirector()
{
	AddCls(GZCLSID_cGZDBSerialRecordAsIGZFile, CreateGZDBSerialRecordAsIGZFile);
	// TODO: SegmentMultiPackedFiles
	AddCls(GZCLSID_cGZDBSegmentPackedFile, CreateGZDBSegmentPackedFile);
	AddCls(GZCLSID_cGZDBSegmentDirectoryFiles, CreateGZDBSegmentDirectoryFiles);
	AddCls(GZCLSID_cGZPersistResourceKeyList, CreateGZResourceKeyList);
	// TODO: GZStringResourceFactory
	// TODO: cSLTokenResourceFactory
	// TODO: cGZWinKeyAcceleratorResFactory
	AddCls(GZCLSID_cGZPersistSurveyorWriter, CreateSurveyorWriter);
	AddCls(GZCLSID_cGZPersistBinaryReader, CreateBinaryReader);
	AddCls(GZCLSID_cGZPersistBinaryWriter, CreateBinaryWriter);
	AddCls(GZCLSID_cGZPersistASCIIWriter, CreateASCIIWriter);
	AddCls(GZCLSID_cRZFastCompression3, CreatecRZFastCompression3);
	AddCls(GZCLSID_cGZPersistBinaryResourceFactory, CreateBinaryResourceFactory);
}

bool cGZResManCOMDirector::OnStart(cIGZCOM* com)
{
	cIGZSystemService* service;
	if (com->GetClassObject(0x656B8F27 /* TODO */, GZIID_cIGZSystemService, reinterpret_cast<void**>(&service)))
	{
		FrameWork()->AddSystemService(service);
		FrameWork()->AddToOnIdle(service);

		if (service != NULL)
		{
			service->Release();
		}
	}

	return true;
}

GZGUID cGZResManCOMDirector::GetDirectorID(void) const
{
	return kGZResManCOMDirectorID;
}

cIGZUnknown* cGZResManCOMDirector::CreateGZDBSegmentDirectoryFiles(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateGZDBSegmentMultiPackedFiles(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateGZDBSegmentPackedFile(void)
{
	return static_cast<cIGZPersistDBSegment*>(new cGZDBSegmentPackedFile());
}

cIGZUnknown* cGZResManCOMDirector::CreateGZDBSerialRecordAsIGZFile(void)
{
	return static_cast<cIGZFile*>(new cGZDBSerialRecordAsIGZFile(NULL));
}

cIGZUnknown* cGZResManCOMDirector::CreateASCIIWriter(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateBinaryReader(void)
{
	return new cGZPersistBinaryReader();
}

cIGZUnknown* cGZResManCOMDirector::CreateBinaryResourceFactory(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateBinaryWriter(void)
{
	return new cGZPersistBinaryWriter();
}

cIGZUnknown* cGZResManCOMDirector::CreateSurveyorWriter(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateGZResourceKeyList(void)
{
	return new cGZPersistResourceKeyList();
}

cIGZUnknown* cGZResManCOMDirector::CreateGZResourceManager(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreateGZStringResourceFactory(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreatecGZWinKeyAcceleratorResFactory(void)
{
	// TODO
	return NULL;
}

cIGZUnknown* cGZResManCOMDirector::CreatecRZFastCompression3(void)
{
	return new cRZFastCompression3();
}

cIGZUnknown* cGZResManCOMDirector::CreatecSLTokenResourceFactory(void)
{
	// TODO
	return NULL;
}

#ifdef GZRESOURCE_EXPORTS
__declspec(dllexport) cIGZCOMDirector* GZDllGetGZCOMDirector(void)
{
	cGZResManCOMDirector director;
	return &director;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif