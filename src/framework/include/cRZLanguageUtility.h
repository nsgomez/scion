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
#include "cRZString.h"

class cRZLanguageUtility : public cIGZLanguageUtility
{
public:
	cRZLanguageUtility();
	virtual ~cRZLanguageUtility() { }

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef();
	virtual uint32_t Release();

public:
	virtual bool Init();
	virtual bool Shutdown();

	virtual bool GetDayName(uint8_t dayOfWeek, cIGZString& outName) const;
	virtual bool GetAbbrDayName(uint8_t dayOfWeek, cIGZString& outName) const;
	virtual bool GetMonthName(uint8_t month, cIGZString& outName) const;
	virtual bool GetAbbrMonthName(uint8_t month, cIGZString& outName) const;

	virtual bool MakeTimeString(int32_t hour, int32_t minute, int32_t second, cIGZString& outTime) const;
	virtual bool MakeDateString(int32_t day, int32_t month, uint32_t year, cIGZString& outDate, GZDateFormat format) const;

	virtual bool GetCurrencySymbol(cIGZString& outSymbol) const;
	virtual bool DoesCurrencySymbolPrecedeAmount() const;
	virtual bool IsSpaceBetweenCurrencySymbolAndAmount() const;

	virtual bool GetDecimalSeparator(cIGZString& outSeparator) const;
	virtual bool GetThousandSeparator(cIGZString& outSeparator) const;

	virtual bool MakeMoneyString(int64_t amount, cIGZString& outMoney, cIGZString const* currencySymbolOverride) const;
	virtual bool MakeFormattedMoneyString(double amount, cIGZString& outMoney, cIGZString& outSymbol, cIGZString const* currencySymbolOverride) const;
	virtual bool MakeNumberString(int64_t value, cIGZString& outNumber) const;
	virtual bool MakeFormattedNumberString(double value, cIGZString& outNumber, cIGZString const& format) const;

	virtual void ConvertToLowerCase(cIGZString const& src, cIGZString& dest) const;
	virtual void ConvertToUpperCase(cIGZString const& src, cIGZString& dest) const;
	virtual int32_t CompareStrings(cIGZString const& first, cIGZString const& second, bool caseSensitive) const;

	virtual int32_t GetCharacterSetFromLanguage() const;
	virtual bool DoesLanguageUseCharacterSet(GZCharacterSet characterSet) const;

	virtual bool GetLanguageRoadDrivingSide() const;
	virtual bool GetMeasurementSystem() const;
	virtual bool AreFirstAndLastNamesReversed() const;
	virtual bool DoesLanguageUseInputMethodEditing() const;
	virtual bool DoesLanguageUseMultiByteCharacters() const;

public:
	virtual void SetLanguage(GZLanguage language);
	virtual void SetCountry(uint32_t country);

protected:
	uint32_t mRefCount;
	uint32_t mLanguage;
	uint32_t mCountry;
	bool mUseLocalCurrency;
	cRZString mNonBreakingSpace;
};