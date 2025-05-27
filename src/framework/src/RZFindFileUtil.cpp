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
#include "RZFindFileUtil.h"

RZFileFindData* RZFileFindFirst(cRZString const& pattern)
{
	WIN32_FIND_DATAA platformFindData;
	HANDLE findHandle = FindFirstFile(pattern.ToChar(), &platformFindData);

	if (findHandle != INVALID_HANDLE_VALUE)
	{
		RZFileFindData* result = new RZFileFindData();
		result->name = cRZString(platformFindData.cFileName);
		result->isDirectory = (platformFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		result->findPattern = pattern;
		result->osHandle = findHandle;

		return result;
	}

	return NULL;
}

bool RZFileFindNext(RZFileFindData* data)
{
	WIN32_FIND_DATAA platformFindData;

	if (data == NULL || !FindNextFile(data->osHandle, &platformFindData))
	{
		return false;
	}

	data->name = cRZString(platformFindData.cFileName);
	data->isDirectory = (platformFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	return true;
}

void RZFileFindFinish(RZFileFindData* data)
{
	if (data != NULL)
	{
		if (data->osHandle != INVALID_HANDLE_VALUE)
		{
			FindClose(data->osHandle);
		}

		delete data;
	}
}