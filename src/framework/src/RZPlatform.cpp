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
#include "RZPlatform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

bool RZIsKeyDownNow(uint32_t key)
{
	if (key == VK_LBUTTON && GetSystemMetrics(SM_SWAPBUTTON) != 0)
	{
		key = VK_RBUTTON;
	}

	return (GetAsyncKeyState(key) >> 15) != 0;
}

int32_t RZThreadSafeAdd(int32_t volatile* addend, int32_t value)
{
	return InterlockedExchangeAdd(reinterpret_cast<LONG volatile*>(addend), value) + value;
}