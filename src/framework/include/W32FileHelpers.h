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
#include <Windows.h>

typedef HANDLE (*pfnCreateFileW)(
		LPCWSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile);

typedef BOOL (*pfnCopyFileW)(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);
typedef BOOL (*pfnMoveFileExW)(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags);
typedef HANDLE (*pfnFindFirstFileW)(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
typedef BOOL (*pfnDeleteFileW)(LPCWSTR lpFileName);
typedef DWORD (*pfnGetFileAttributesW)(LPCWSTR lpFileName);
typedef BOOL (*pfnSetFileAttributesW)(LPCWSTR lpFileName, DWORD dwFileAttributes);
typedef DWORD (*pfnGetLongPathNameA)(LPCSTR lpszShortPath, LPSTR lpszLongPath, DWORD cchBuffer);
typedef DWORD (*pfnGetLongPathNameW)(LPCWSTR lpszShortPath, LPWSTR lpszLongPath, DWORD cchBuffer);
typedef DWORD (*pfnGetTempPathW)(DWORD nBufferLength, LPWSTR lpBuffer);

typedef BOOL (*pfnGetDiskFreeSpaceExA)(
	LPCSTR lpDirectoryName,
	PULARGE_INTEGER lpFreeBytesAvailableToCaller,
	PULARGE_INTEGER lpTotalNumberOfBytes,
	PULARGE_INTEGER lpTotalNumberOfFreeBytes);

typedef BOOL (*pfnGetDiskFreeSpaceExW)(
	LPCWSTR lpDirectoryName,
	PULARGE_INTEGER lpFreeBytesAvailableToCaller,
	PULARGE_INTEGER lpTotalNumberOfBytes,
	PULARGE_INTEGER lpTotalNumberOfFreeBytes);

struct W32FileHelpers
{
public:
	W32FileHelpers();

public:
	pfnCreateFileW CreateFileW;
	pfnCopyFileW CopyFileW;
	pfnMoveFileExW MoveFileExW;
	pfnFindFirstFileW FindFirstFileW;
	pfnDeleteFileW DeleteFileW;
	pfnGetFileAttributesW GetFileAttributesW;
	pfnSetFileAttributesW SetFileAttributesW;
	pfnGetLongPathNameA GetLongPathNameA;
	pfnGetLongPathNameW GetLongPathNameW;
	pfnGetTempPathW GetTempPathW;
	pfnGetDiskFreeSpaceExA GetDiskFreeSpaceExA;
	pfnGetDiskFreeSpaceExW GetDiskFreeSpaceExW;

public:
	bool mInitialized;
};

extern W32FileHelpers gW32FileHelpers;