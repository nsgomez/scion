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
#include <stdint.h>

class cIGZString;

#define GZ_CODEPAGE_SYSTEM  -1
#define GZ_CODEPAGE_NATIVE  -2
#define GZ_CODEPAGE_UTF8    8
#define GZ_CODEPAGE_UTF16   16

extern void ConvertStringEncoding(cIGZString const& src, void* dest, uint32_t& destLength, int32_t codePage);
extern void ConvertStringEncoding(void const* src, uint32_t srcLength, int32_t codePage, cIGZString& dest);