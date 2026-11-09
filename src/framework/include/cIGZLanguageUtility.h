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
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZLanguageUtility = 0x1054B7D5;

class cIGZString;

enum GZDateFormat
{
	kGZDateFormat_Short,
	kGZDateFormat_Medium,
	kGZDateFormat_Long,
	kGZDateFormat_Full,
	kGZDateFormat_LongAbbr,
	kGZDateFormat_FullAbbr,
};

enum GZCountry
{
	kGZCountry_Albania       = 1,
	kGZCountry_Argentina     = 2,
	kGZCountry_Australia     = 3,
	kGZCountry_Austria       = 4,
	kGZCountry_Belgium       = 5,
	kGZCountry_Brazil        = 6,
	kGZCountry_Bulgaria      = 7,
	kGZCountry_Canada        = 8,
	kGZCountry_Chile         = 9,
	kGZCountry_China         = 10,
	kGZCountry_Colombia      = 11,
	kGZCountry_Cuba          = 12,
	kGZCountry_CzechRepublic = 13,
	kGZCountry_Denmark       = 14,
	kGZCountry_Egypt         = 15,
	kGZCountry_Finland       = 16,
	kGZCountry_France        = 17,
	kGZCountry_Georgia       = 18,
	kGZCountry_Germany       = 19,
	kGZCountry_Greece        = 20,
	kGZCountry_HolySee       = 21, // You never know if the Pope might want to play
	kGZCountry_HongKong      = 22,
	kGZCountry_Hungary       = 23,
	kGZCountry_Iceland       = 24,
	kGZCountry_India         = 25,
	kGZCountry_Iran          = 26,
	kGZCountry_Iraq          = 27,
	kGZCountry_Ireland       = 28,
	kGZCountry_Israel        = 29,
	kGZCountry_Italy         = 30,
	kGZCountry_Japan         = 31,
	kGZCountry_Korea         = 32,
	kGZCountry_Mexico        = 33,
	kGZCountry_Netherlands   = 34,
	kGZCountry_NewZealand    = 35,
	kGZCountry_Norway        = 36,
	kGZCountry_Philippines   = 37,
	kGZCountry_Poland        = 38,
	kGZCountry_Portugal      = 39,
	kGZCountry_Romania       = 40,
	kGZCountry_Russia        = 41,
	kGZCountry_Singapore     = 42,
	kGZCountry_Slovakia      = 43,
	kGZCountry_Slovenia      = 44,
	kGZCountry_Spain         = 45,
	kGZCountry_Sweden        = 46,
	kGZCountry_Switzerland   = 47,
	kGZCountry_Syria         = 48,
	kGZCountry_Taiwan        = 49,
	kGZCountry_Thailand      = 50,
	kGZCountry_Turkey        = 51,
	kGZCountry_Ukraine       = 52,
	kGZCountry_UnitedKingdom = 53,
	kGZCountry_UnitedStates  = 54,
	kGZCountry_Vietnam       = 55,
	kGZCountry_Yugoslavia    = 56, // When did they write this code??
};

enum GZLanguage
{
	kGZLanguage_English    = 1,
	kGZLanguage_English_UK = 2,
	kGZLanguage_French     = 3,
	kGZLanguage_German     = 4,
	kGZLanguage_Italian    = 5,
	kGZLanguage_Spanish    = 6,
	kGZLanguage_Dutch      = 7,
	kGZLanguage_Danish     = 8,
	kGZLanguage_Swedish    = 9,
	kGZLanguage_Norwegian  = 10,
	kGZLanguage_Finnish    = 11,
	kGZLanguage_Hebrew     = 12,
	kGZLanguage_Russian    = 13,
	kGZLanguage_Portuguese = 14,
	kGZLanguage_Japanese   = 15,
	kGZLanguage_Polish     = 16,
	kGZLanguage_Chinese_Simplified  = 17,
	kGZLanguage_Chinese_Traditional = 18,
	kGZLanguage_Thai       = 19,
	kGZLanguage_Korean     = 20,
	kGZLanguage_Hindi      = 21,
	kGZLanguage_Arabic     = 22,
	kGZLanguage_Bulgarian  = 23,
	kGZLanguage_Cyrillic   = 24,
	kGZLanguage_Ukrainian  = 25,
	kGZLanguage_Czech      = 26,
	kGZLanguage_Greek      = 27,
	kGZLanguage_Hungarian  = 28,
	kGZLanguage_Icelandic  = 29,
	kGZLanguage_Romanian   = 30,
	kGZLanguage_Latin      = 31,
	kGZLanguage_Slovak     = 32,
	kGZLanguage_Albanian   = 33,
	kGZLanguage_Turkish    = 34,
	kGZLanguage_Portuguese_Brazilian = 35,
	kGZLanguage_French_Swiss         = 36,
	kGZLanguage_French_Canadian      = 37,
	kGZLanguage_French_Belgian       = 38,
	kGZLanguage_German_Swiss         = 39,
	kGZLanguage_Italian_Swiss        = 40,
	kGZLanguage_Dutch_Belgian        = 41,
	kGZLanguage_Spanish_Mexican      = 42,
	kGZLanguage_Tagalog    = 43,
	kGZLanguage_Vietnamese = 44,
};

enum GZCharacterSet
{
	kGZCharSet_Thai               = 874,
	kGZCharSet_JapaneseShiftJIS   = 932,
	kGZCharSet_SimplifiedChinese  = 936,
	kGZCharSet_Korean             = 949,
	kGZCharSet_TraditionalChinese = 950,
	kGZCharSet_CentralEurope      = 1250,
	kGZCharSet_Cyrillic           = 1251,
	kGZCharSet_ANSI               = 1252,
	kGZCharSet_Greek              = 1253,
	kGZCharSet_Turkish            = 1254,
	kGZCharSet_Hebrew             = 1255,
	kGZCharSet_Arabic             = 1256,
	kGZCharSet_Baltic             = 1257,
};

class cIGZLanguageUtility : public cIGZUnknown
{
public:
	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;

	virtual bool GetDayName(uint8_t dayOfWeek, cIGZString& outName) const = 0;
	virtual bool GetAbbrDayName(uint8_t dayOfWeek, cIGZString& outName) const = 0;
	virtual bool GetMonthName(uint8_t month, cIGZString& outName) const = 0;
	virtual bool GetAbbrMonthName(uint8_t month, cIGZString& outName) const = 0;

	virtual bool MakeTimeString(int32_t hour, int32_t minute, int32_t second, cIGZString& outTime) const = 0;
	virtual bool MakeDateString(int32_t day, int32_t month, uint32_t year, cIGZString& outDate, GZDateFormat format) const = 0;

	virtual bool GetCurrencySymbol(cIGZString& outSymbol) const = 0;
	virtual bool DoesCurrencySymbolPrecedeAmount() const = 0;
	virtual bool IsSpaceBetweenCurrencySymbolAndAmount() const = 0;

	virtual bool GetDecimalSeparator(cIGZString& outSeparator) const = 0;
	virtual bool GetThousandSeparator(cIGZString& outSeparator) const = 0;

	virtual bool MakeMoneyString(int64_t amount, cIGZString& outMoney, cIGZString const* currencySymbolOverride) const = 0;
	virtual bool MakeFormattedMoneyString(double amount, cIGZString& outMoney, cIGZString& outSymbol, cIGZString const* currencySymbolOverride) const = 0;
	virtual bool MakeNumberString(int64_t value, cIGZString& outNumber) const = 0;
	virtual bool MakeFormattedNumberString(double value, cIGZString& outNumber, cIGZString const& format) const = 0;

	virtual void ConvertToLowerCase(cIGZString const& src, cIGZString& dest) const = 0;
	virtual void ConvertToUpperCase(cIGZString const& src, cIGZString& dest) const = 0;
	virtual int32_t CompareStrings(cIGZString const& first, cIGZString const& second, bool caseSensitive) const = 0;

	virtual int32_t GetCharacterSetFromLanguage() const = 0;
	virtual bool DoesLanguageUseCharacterSet(GZCharacterSet characterSet) const = 0;

	virtual bool GetLanguageRoadDrivingSide() const = 0;
	virtual bool GetMeasurementSystem() const = 0;
	virtual bool AreFirstAndLastNamesReversed() const = 0;
	virtual bool DoesLanguageUseInputMethodEditing() const = 0;
	virtual bool DoesLanguageUseMultiByteCharacters() const = 0;
};