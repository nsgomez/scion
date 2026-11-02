/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2021  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include <stdio.h>
#include <Windows.h>
#include "cRZExceptionNotification.h"

cRZExceptionNotification::cRZExceptionNotification()
{
}

cRZExceptionNotification::~cRZExceptionNotification()
{
}

bool cRZExceptionNotification::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZExceptionNotification)
	{
		*outPtr = static_cast<cIGZExceptionNotification*>(this);
	}
	else if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
	}
	else
	{
		return false;
	}

	AddRef();
	return false;
}

uint32_t cRZExceptionNotification::AddRef()
{
	return ++mRefCount;
}

uint32_t cRZExceptionNotification::Release()
{
	if (mRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		return --mRefCount;
	}
}

bool cRZExceptionNotification::Notify(char const* exceptionText, int address)
{
	char message[200];
	int length = sizeof(message);

	__try {
		_snprintf(message, length, "Module: %s\nLine: %d",
			(exceptionText != NULL) ? exceptionText : "not specified",
			address);
		message[length - 1] = '\0';

		return MessageBox(NULL, message, "Exception about to be thrown, accept?", MB_YESNO | MB_ICONEXCLAMATION) == IDYES;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return true;
	}
}