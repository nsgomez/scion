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
#include <vector>

class cIGZString;
class cRZString;

enum GZCodePage
{
	kGZCodePageInvalid     = -3,
	kGZCodePageNative      = -2,
	kGZCodePageSystem      = -1,
	kGZCodePageANSI        = 0,
	kGZCodePageUTF8        = 8,
	kGZCodePageUTF16       = 16,
#ifdef WIN32
	kGZCodePageOEMJapanese           = 932,
	kGZCodePageOEMSimplifiedChinese  = 936,
	kGZCodePageOEMKorean             = 949,
	kGZCodePageOEMTraditionalChinese = 950,
#endif
};

uint32_t ConvertStringEncoding(void const* src, uint32_t srcLength, GZCodePage srcCodePage, void* dest, uint32_t& destLength, GZCodePage destCodePage);
extern uint32_t ConvertStringEncoding(void const* src, uint32_t srcLength, GZCodePage codePage, cIGZString& dest);
extern uint32_t ConvertStringEncoding(cIGZString const& src, void* dest, uint32_t& destLength, GZCodePage codePage);
extern bool ConvertStringEncoding(cIGZString const& src, GZCodePage srcCodePage, cIGZString& dest);
extern bool ConvertStringEncoding(cIGZString const& src, cIGZString& dest, GZCodePage destCodePage);
extern int32_t GetCharLength(void const* src, uint32_t srcLength, GZCodePage codePage);
extern GZCodePage GetCurrentFrameworkCodePage();
extern GZCodePage GetCurrentSystemCodePage();
extern uint32_t GetGlyphCountFromString(void const* src, uint32_t srcLength, GZCodePage codePage);
extern bool IsLeadByte(char ch, GZCodePage codePage);
extern void LimitStringLength(cRZString& str, uint32_t maxBytes);
extern uint32_t RZAtoUint32HexA(char const*, ...);
extern std::vector<wchar_t> RZStringToUTF16(cIGZString const& src);