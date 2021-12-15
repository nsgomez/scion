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
#include <Windows.h>
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZFrameworkW32 = 0x23EA70A1;

class cIGZFramework;

class cIGZFrameworkW32 : public cIGZUnknown
{
public:
	virtual cIGZFramework* AsIGZFramework(void) = 0;
	virtual void Run(void) = 0;

	virtual HINSTANCE GetWindowsInstance(void) = 0;
	virtual HWND GetMainHWND(void) = 0;
	virtual void SetMainHWND(HWND hwnd) = 0;
};