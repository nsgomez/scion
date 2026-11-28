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
#include "cIGZLanguageUtility.h"

static const GZIID GZIID_cIGZLanguageManager = 0x0054B7D5;
static const GZGUID RZSRVID_cRZLanguageManager = 0x441E5070;

class cIGZString;

class cIGZLanguageManager : public cIGZUnknown
{
public:
	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;

	virtual bool AddAvailableLanguage(GZLanguage language) = 0;
	virtual bool RemoveAvailableLanguage(GZLanguage language) = 0;

	virtual GZLanguage GetCurrentLanguage() = 0;
	virtual bool SetCurrentLanguage(GZLanguage language) = 0;
	virtual bool CanSwitchToLanguage(GZLanguage language) = 0;

	virtual uint32_t GetCurrentSystemLanguage() = 0;
	virtual uint32_t GetSystemLanguageFromLanguage(GZLanguage language) = 0;
	virtual GZLanguage GetLanguageFromSystemLanguage(uint32_t systemLanguage) = 0;

	virtual bool GetLanguageRuntimeLibraryName(cIGZString& name, GZLanguage language) = 0;
	virtual bool GetNextAvailableLanguage(GZLanguage& language) = 0;

	virtual bool GetLanguageDirectoryName(cIGZString& name, GZLanguage language) = 0;
	virtual bool GetLanguageEnglishName(cIGZString& name, GZLanguage language) = 0;

	virtual bool GetLanguageIDFromLanguageEnglishName(cIGZString const& name, GZLanguage& language) = 0;
	virtual bool GetLanguageIDFromLanguageLocalName(cIGZString const& name, GZLanguage& language) = 0;
	virtual bool GetLanguageIDFromEitherEnglishOrLocalName(cIGZString const& name, GZLanguage& language) = 0;

	virtual bool GetLanguageEnglishNameFromLanguageID(cIGZString& name, GZLanguage language) = 0;
	virtual bool GetLanguageLocalNameFromLanguageID(cIGZString& name, GZLanguage language) = 0;
};