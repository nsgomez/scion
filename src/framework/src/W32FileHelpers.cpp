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

#include "W32FileHelpers.h"

W32FileHelpers::W32FileHelpers()
{
	HMODULE hModule = GetModuleHandle("kernel32.dll");
	this->GetLongPathNameA = reinterpret_cast<pfnGetLongPathNameA>(GetProcAddress(hModule, "GetLongPathNameA"));
	this->GetDiskFreeSpaceExA = reinterpret_cast<pfnGetDiskFreeSpaceExA>(GetProcAddress(hModule, "GetDiskFreeSpaceExA"));

	if (GetVersion() >= 0x80000000)
	{
		this->CreateFileW = NULL;
		this->CopyFileW = NULL;
		this->MoveFileExW = NULL;
		this->FindFirstFileW = NULL;
		this->DeleteFileW = NULL;
		this->GetFileAttributesW = NULL;
		this->SetFileAttributesW = NULL;
		this->GetLongPathNameW = NULL;
		this->GetTempPathW = NULL;
		this->GetDiskFreeSpaceExW = NULL;

		this->mInitialized = false;
	}
	else
	{
		this->CreateFileW = reinterpret_cast<pfnCreateFileW>(GetProcAddress(hModule, "CreateFileW"));
		this->CopyFileW = reinterpret_cast<pfnCopyFileW>(GetProcAddress(hModule, "CopyFileW"));
		this->MoveFileExW = reinterpret_cast<pfnMoveFileExW>(GetProcAddress(hModule, "MoveFileExW"));
		this->FindFirstFileW = reinterpret_cast<pfnFindFirstFileW>(GetProcAddress(hModule, "FindFirstFileW"));
		this->DeleteFileW = reinterpret_cast<pfnDeleteFileW>(GetProcAddress(hModule, "DeleteFileW"));
		this->GetFileAttributesW = reinterpret_cast<pfnGetFileAttributesW>(GetProcAddress(hModule, "GetFileAttributesW"));
		this->SetFileAttributesW = reinterpret_cast<pfnSetFileAttributesW>(GetProcAddress(hModule, "SetFileAttributesW"));
		this->GetLongPathNameW = reinterpret_cast<pfnGetLongPathNameW>(GetProcAddress(hModule, "GetLongPathNameW"));
		this->GetTempPathW = reinterpret_cast<pfnGetTempPathW>(GetProcAddress(hModule, "GetTempPathW"));
		this->GetDiskFreeSpaceExW = reinterpret_cast<pfnGetDiskFreeSpaceExW>(GetProcAddress(hModule, "GetDiskFreeSpaceExW"));

		this->mInitialized = true;
	}
}

W32FileHelpers gW32FileHelpers;