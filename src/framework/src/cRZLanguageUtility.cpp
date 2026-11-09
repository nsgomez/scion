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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "cRZLanguageUtility.h"
#include "RZStringUtil.h"

cRZLanguageUtility::cRZLanguageUtility() :
	mRefCount(0),
	mLanguage(kGZLanguage_English),
	mCountry(kGZCountry_UnitedStates),
	mUseLocalCurrency(false),
	mNonBreakingSpace("\xc2\xa0")
{
}

bool cRZLanguageUtility::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	case GZIID_cIGZLanguageUtility:
		*outPtr = static_cast<cIGZLanguageUtility*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZLanguageUtility::AddRef()
{
	return ++mRefCount;
}

uint32_t cRZLanguageUtility::Release()
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}

	delete this;
	return 0;
}

bool cRZLanguageUtility::Init()
{
	mNonBreakingSpace = cRZString("\xc2\xa0");
	ConvertUTF8StringToDestinationFormat(mNonBreakingSpace);
	return true;
}

bool cRZLanguageUtility::Shutdown()
{
	return true;
}

bool cRZLanguageUtility::GetDayName(uint8_t dayOfWeek, cIGZString& outName) const
{
	bool translated = true;

	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_French:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Lundi"); break;
		case 2: outName.FromChar("Mardi"); break;
		case 3: outName.FromChar("Mercredi"); break;
		case 4: outName.FromChar("Jeudi"); break;
		case 5: outName.FromChar("Vendredi"); break;
		case 6: outName.FromChar("Samedi"); break;
		case 7: outName.FromChar("Dimanche"); break;
		}
		break;

	case kGZLanguage_German:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Montag"); break;
		case 2: outName.FromChar("Dienstag"); break;
		case 3: outName.FromChar("Mittwoch"); break;
		case 4: outName.FromChar("Donnerstag"); break;
		case 5: outName.FromChar("Freitag"); break;
		case 6: outName.FromChar("Samstag"); break;
		case 7: outName.FromChar("Sonntag"); break;
		}
		break;

	case kGZLanguage_Italian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Luned\xc3\xac"); break;
		case 2: outName.FromChar("Marted\xc3\xac"); break;
		case 3: outName.FromChar("Mercoled\xc3\xac"); break;
		case 4: outName.FromChar("Gioved\xc3\xac"); break;
		case 5: outName.FromChar("Venerd\xc3\xac"); break;
		case 6: outName.FromChar("Sabato"); break;
		case 7: outName.FromChar("Domenica"); break;
		}
		break;

	case kGZLanguage_Spanish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Lunes"); break;
		case 2: outName.FromChar("Martes"); break;
		case 3: outName.FromChar("Mi\xc3\xa9rcoles"); break;
		case 4: outName.FromChar("Jueves"); break;
		case 5: outName.FromChar("Viernes"); break;
		case 6: outName.FromChar("S\xc3\xa1" "bado"); break;
		case 7: outName.FromChar("Domingo"); break;
		}
		break;

	case kGZLanguage_Dutch:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Maandag"); break;
		case 2: outName.FromChar("Dinsdag"); break;
		case 3: outName.FromChar("Woensdag"); break;
		case 4: outName.FromChar("Donderdag"); break;
		case 5: outName.FromChar("Vrijdag"); break;
		case 6: outName.FromChar("Zaterdag"); break;
		case 7: outName.FromChar("Zondag"); break;
		}
		break;

	case kGZLanguage_Danish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Mandag"); break;
		case 2: outName.FromChar("Tirsdag"); break;
		case 3: outName.FromChar("Onsdag"); break;
		case 4: outName.FromChar("Torsdag"); break;
		case 5: outName.FromChar("Fredag"); break;
		case 6: outName.FromChar("L\xc3\xb8rdag"); break;
		case 7: outName.FromChar("S\xc3\xb8ndag"); break;
		}
		break;

	case kGZLanguage_Swedish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_Norwegian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Mandag"); break;
		case 2: outName.FromChar("Tirsdag"); break;
		case 3: outName.FromChar("Onsdag"); break;
		case 4: outName.FromChar("Torsdag"); break;
		case 5: outName.FromChar("Fredag"); break;
		case 6: outName.FromChar("L\xc3\xb8rdag"); break;
		case 7: outName.FromChar("S\xc3\xb8ndag"); break;
		}
		break;

	case kGZLanguage_Finnish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Maanantai"); break;
		case 2: outName.FromChar("Tiistai"); break;
		case 3: outName.FromChar("Keskiviikko"); break;
		case 4: outName.FromChar("Torstai"); break;
		case 5: outName.FromChar("Perjantai"); break;
		case 6: outName.FromChar("Lauantai"); break;
		case 7: outName.FromChar("Sunnuntai"); break;
		}
		break;

	case kGZLanguage_Hebrew:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_Russian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_Portuguese:
	case kGZLanguage_Portuguese_Brazilian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Segunda-feira"); break;
		case 2: outName.FromChar("Ter\xe7" "a-feira"); break;
		case 3: outName.FromChar("Quarta-feira"); break;
		case 4: outName.FromChar("Quinta-feira"); break;
		case 5: outName.FromChar("Sexta-feira"); break;
		case 6: outName.FromChar("S\xe1" "bado"); break;
		case 7: outName.FromChar("Domingo"); break;
		}
		break;

	case kGZLanguage_Japanese:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x9c\x88\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 2: outName.FromChar("\xe7\x81\xab\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 3: outName.FromChar("\xe6\xb0\xb4\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 4: outName.FromChar("\xe6\x9c\xa8\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 5: outName.FromChar("\xe9\x87\x91\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 6: outName.FromChar("\xe5\x9c\x9f\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 7: outName.FromChar("\xe6\x97\xa5\xe6\x9b\x9c\xe6\x97\xa5"); break;
		}
		break;

	case kGZLanguage_Polish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_Chinese_Simplified:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x80"); break;
		case 2: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x8c"); break;
		case 3: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x89"); break;
		case 4: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x9b\x9b"); break;
		case 5: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x94"); break;
		case 6: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x85\xad"); break;
		case 7: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\xa4\xa9"); break;
		}
		break;

	case kGZLanguage_Chinese_Traditional:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x80"); break;
		case 2: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x8c"); break;
		case 3: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x89"); break;
		case 4: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x9b\x9b"); break;
		case 5: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x94"); break;
		case 6: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x85\xad"); break;
		case 7: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\xa4\xa9"); break;
		}
		break;

	case kGZLanguage_Thai:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Monday"); break;
		case 2: outName.FromChar("Tuesday"); break;
		case 3: outName.FromChar("Wednesday"); break;
		case 4: outName.FromChar("Thursday"); break;
		case 5: outName.FromChar("Friday"); break;
		case 6: outName.FromChar("Saturday"); break;
		case 7: outName.FromChar("Sunday"); break;
		}
		break;

	case kGZLanguage_Korean:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xec\x9b\x94\xec\x9a\x94\xec\x9d\xbc"); break;
		case 2: outName.FromChar("\xed\x99\x94\xec\x9a\x94\xec\x9d\xbc"); break;
		case 3: outName.FromChar("\xec\x88\x98\xec\x9a\x94\xec\x9d\xbc"); break;
		case 4: outName.FromChar("\xeb\xaa\xa9\xec\x9a\x94\xec\x9d\xbc"); break;
		case 5: outName.FromChar("\xea\xb8\x88\xec\x9a\x94\xec\x9d\xbc"); break;
		case 6: outName.FromChar("\xed\x86\xa0\xec\x9a\x94\xec\x9d\xbc"); break;
		case 7: outName.FromChar("\xec\x9d\xbc\xec\x9a\x94\xec\x9d\xbc"); break;
		}
		break;
	}

	if (outName.Strlen() == 0)
	{
		outName.FromChar("Untranslated");
		translated = false;
	}

	ConvertUTF8StringToDestinationFormat(outName);
	return translated;
}

bool cRZLanguageUtility::GetAbbrDayName(uint8_t dayOfWeek, cIGZString& outName) const
{
	bool translated = true;

	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Mon"); break;
		case 2: outName.FromChar("Tue"); break;
		case 3: outName.FromChar("Wed"); break;
		case 4: outName.FromChar("Thur"); break;
		case 5: outName.FromChar("Fri"); break;
		case 6: outName.FromChar("Sat"); break;
		case 7: outName.FromChar("Sun"); break;
		}
		break;

	case kGZLanguage_French:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("lun"); break;
		case 2: outName.FromChar("mar"); break;
		case 3: outName.FromChar("mer"); break;
		case 4: outName.FromChar("jeu"); break;
		case 5: outName.FromChar("ven"); break;
		case 6: outName.FromChar("sam"); break;
		case 7: outName.FromChar("dim"); break;
		}
		break;

	case kGZLanguage_German:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Mo"); break;
		case 2: outName.FromChar("Di"); break;
		case 3: outName.FromChar("Mi"); break;
		case 4: outName.FromChar("Do"); break;
		case 5: outName.FromChar("Fr"); break;
		case 6: outName.FromChar("Sa"); break;
		case 7: outName.FromChar("So"); break;
		}
		break;

	case kGZLanguage_Italian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Lun"); break;
		case 2: outName.FromChar("Mar"); break;
		case 3: outName.FromChar("Mer"); break;
		case 4: outName.FromChar("Gio"); break;
		case 5: outName.FromChar("Ven"); break;
		case 6: outName.FromChar("Sab"); break;
		case 7: outName.FromChar("Dom"); break;
		}
		break;

	case kGZLanguage_Spanish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("L"); break;
		case 2: outName.FromChar("M"); break;
		case 3: outName.FromChar("X"); break;
		case 4: outName.FromChar("J"); break;
		case 5: outName.FromChar("V"); break;
		case 6: outName.FromChar("S"); break;
		case 7: outName.FromChar("D"); break;
		}
		break;

	case kGZLanguage_Dutch:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Ma"); break;
		case 2: outName.FromChar("Di"); break;
		case 3: outName.FromChar("Wo"); break;
		case 4: outName.FromChar("Do"); break;
		case 5: outName.FromChar("Vr"); break;
		case 6: outName.FromChar("Za"); break;
		case 7: outName.FromChar("Zo"); break;
		}
		break;

	case kGZLanguage_Swedish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("M\xe5\x06" "E"); break;
		case 2: outName.FromChar("Tis"); break;
		case 3: outName.FromChar("Ons"); break;
		case 4: outName.FromChar("Tor"); break;
		case 5: outName.FromChar("Fre"); break;
		case 6: outName.FromChar("L\xf6r"); break;
		case 7: outName.FromChar("S\xf6n"); break;
		}
		break;

	case kGZLanguage_Finnish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("ma"); break;
		case 2: outName.FromChar("ti"); break;
		case 3: outName.FromChar("ke"); break;
		case 4: outName.FromChar("to"); break;
		case 5: outName.FromChar("pe"); break;
		case 6: outName.FromChar("la"); break;
		case 7: outName.FromChar("su"); break;
		}
		break;

	case kGZLanguage_Russian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xcf\xed"); break;
		case 2: outName.FromChar("\xc4\xf2"); break;
		case 3: outName.FromChar("\xd1\xf0"); break;
		case 4: outName.FromChar("\xd7\xf2"); break;
		case 5: outName.FromChar("\xcf\xf2"); break;
		case 6: outName.FromChar("\xd1\xe1"); break;
		case 7: outName.FromChar("\xc4\xf1"); break;
		}
		break;

	case kGZLanguage_Portuguese:
	case kGZLanguage_Portuguese_Brazilian:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Seg"); break;
		case 2: outName.FromChar("Ter"); break;
		case 3: outName.FromChar("Qua"); break;
		case 4: outName.FromChar("Qui"); break;
		case 5: outName.FromChar("Sex"); break;
		case 6: outName.FromChar("Sab"); break;
		case 7: outName.FromChar("Dom"); break;
		}
		break;

	case kGZLanguage_Japanese:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x9c\x88\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 2: outName.FromChar("\xe7\x81\xab\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 3: outName.FromChar("\xe6\xb0\xb4\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 4: outName.FromChar("\xe6\x9c\xa8\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 5: outName.FromChar("\xe9\x87\x91\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 6: outName.FromChar("\xe5\x9c\x9f\xe6\x9b\x9c\xe6\x97\xa5"); break;
		case 7: outName.FromChar("\xe6\x97\xa5\xe6\x9b\x9c\xe6\x97\xa5"); break;
		}
		break;

	case kGZLanguage_Polish:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("Pn"); break;
		case 2: outName.FromChar("Wt"); break;
		case 3: outName.FromChar("\xc2\x8cr"); break;
		case 4: outName.FromChar("Cz"); break;
		case 5: outName.FromChar("Pt"); break;
		case 6: outName.FromChar("So"); break;
		case 7: outName.FromChar("N"); break;
		}
		break;

	case kGZLanguage_Chinese_Simplified:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x80"); break;
		case 2: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x8c"); break;
		case 3: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x89"); break;
		case 4: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x9b\x9b"); break;
		case 5: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x94"); break;
		case 6: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x85\xad"); break;
		case 7: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\xa4\xa9"); break;
		}
		break;

	case kGZLanguage_Chinese_Traditional:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x80"); break;
		case 2: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x8c"); break;
		case 3: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x89"); break;
		case 4: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x9b\x9b"); break;
		case 5: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x94"); break;
		case 6: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\x85\xad"); break;
		case 7: outName.FromChar("\xe6\x98\x9f\xe6\x9c\x9f\xe5\xa4\xa9"); break;
		}
		break;

	case kGZLanguage_Thai:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xa8"); break;
		case 2: outName.FromChar("\xcd"); break;
		case 3: outName.FromChar("\xbe"); break;
		case 4: outName.FromChar("\xbe\xc4"); break;
		case 5: outName.FromChar("\xc9"); break;
		case 6: outName.FromChar("\xca"); break;
		case 7: outName.FromChar("\xcd\xd2"); break;
		}
		break;

	case kGZLanguage_Korean:
		switch (dayOfWeek)
		{
		case 1: outName.FromChar("\xec\x9b\x94\xec\x9a\x94\xec\x9d\xbc"); break;
		case 2: outName.FromChar("\xed\x99\x94\xec\x9a\x94\xec\x9d\xbc"); break;
		case 3: outName.FromChar("\xec\x88\x98\xec\x9a\x94\xec\x9d\xbc"); break;
		case 4: outName.FromChar("\xeb\xaa\xa9\xec\x9a\x94\xec\x9d\xbc"); break;
		case 5: outName.FromChar("\xea\xb8\x88\xec\x9a\x94\xec\x9d\xbc"); break;
		case 6: outName.FromChar("\xed\x86\xa0\xec\x9a\x94\xec\x9d\xbc"); break;
		case 7: outName.FromChar("\xec\x9d\xbc\xec\x9a\x94\xec\x9d\xbc"); break;
		}
		break;
	}

	if (outName.Strlen() == 0)
	{
		outName.FromChar("Untranslated");
		translated = false;
	}

	ConvertUTF8StringToDestinationFormat(outName);
	return translated;
}

bool cRZLanguageUtility::GetMonthName(uint8_t month, cIGZString& outName) const
{
	bool translated = true;

	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
		switch (month)
		{
		case 1: outName.FromChar("January"); break;
		case 2: outName.FromChar("February"); break;
		case 3: outName.FromChar("March"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("June"); break;
		case 7: outName.FromChar("July"); break;
		case 8: outName.FromChar("August"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("October"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_French:
		switch (month)
		{
		case 1: outName.FromChar("janvier"); break;
		case 2: outName.FromChar("F\xe9vrier"); break;
		case 3: outName.FromChar("mars"); break;
		case 4: outName.FromChar("avril"); break;
		case 5: outName.FromChar("mai"); break;
		case 6: outName.FromChar("juin"); break;
		case 7: outName.FromChar("juillet"); break;
		case 8: outName.FromChar("Ao\xfbt"); break;
		case 9: outName.FromChar("september"); break;
		case 10: outName.FromChar("october"); break;
		case 11: outName.FromChar("november"); break;
		case 12: outName.FromChar("D\xe9" "cembre"); break;
		}
		break;

	case kGZLanguage_German:
		switch (month)
		{
		case 1: outName.FromChar("Januar"); break;
		case 2: outName.FromChar("Februar"); break;
		case 3: outName.FromChar("M\xe4rz"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("Mai"); break;
		case 6: outName.FromChar("Juni"); break;
		case 7: outName.FromChar("Juli"); break;
		case 8: outName.FromChar("August"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("Oktober"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("Dezember"); break;
		}
		break;

	case kGZLanguage_Italian:
		switch (month)
		{
		case 1: outName.FromChar("Gennaio"); break;
		case 2: outName.FromChar("Febbraio"); break;
		case 3: outName.FromChar("Marzo"); break;
		case 4: outName.FromChar("Aprile"); break;
		case 5: outName.FromChar("Maggio"); break;
		case 6: outName.FromChar("Giugno"); break;
		case 7: outName.FromChar("Luglio"); break;
		case 8: outName.FromChar("Agosto"); break;
		case 9: outName.FromChar("Settembre"); break;
		case 10: outName.FromChar("Ottobre"); break;
		case 11: outName.FromChar("Novembre"); break;
		case 12: outName.FromChar("Dicembre"); break;
		}
		break;

	case kGZLanguage_Spanish:
		switch (month)
		{
		case 1: outName.FromChar("Enero"); break;
		case 2: outName.FromChar("Febrero"); break;
		case 3: outName.FromChar("Marzo"); break;
		case 4: outName.FromChar("Abril"); break;
		case 5: outName.FromChar("Mayo"); break;
		case 6: outName.FromChar("Junio"); break;
		case 7: outName.FromChar("Julio"); break;
		case 8: outName.FromChar("Agosto"); break;
		case 9: outName.FromChar("Septiembre"); break;
		case 10: outName.FromChar("Octubre"); break;
		case 11: outName.FromChar("Noviembre"); break;
		case 12: outName.FromChar("Diciembre"); break;
		}
		break;

	case kGZLanguage_Dutch:
		switch (month)
		{
		case 1: outName.FromChar("Januari"); break;
		case 2: outName.FromChar("Februari"); break;
		case 3: outName.FromChar("Maart"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("Mei"); break;
		case 6: outName.FromChar("Juni"); break;
		case 7: outName.FromChar("Juli"); break;
		case 8: outName.FromChar("Augustus"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("Oktober"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_Danish:
		switch (month)
		{
		case 1: outName.FromChar("januar"); break;
		case 2: outName.FromChar("februar"); break;
		case 3: outName.FromChar("marts"); break;
		case 4: outName.FromChar("april"); break;
		case 5: outName.FromChar("maj"); break;
		case 6: outName.FromChar("juni"); break;
		case 7: outName.FromChar("juli"); break;
		case 8: outName.FromChar("august"); break;
		case 9: outName.FromChar("september"); break;
		case 10: outName.FromChar("oktober"); break;
		case 11: outName.FromChar("november"); break;
		case 12: outName.FromChar("december"); break;
		}
		break;

	case kGZLanguage_Swedish:
		switch (month)
		{
		case 1: outName.FromChar("Januari"); break;
		case 2: outName.FromChar("Februari"); break;
		case 3: outName.FromChar("Mars"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("Maj"); break;
		case 6: outName.FromChar("Juni"); break;
		case 7: outName.FromChar("Juli"); break;
		case 8: outName.FromChar("Augusti"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("Oktober"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_Finnish:
		switch (month)
		{
		case 1: outName.FromChar("tamikuu"); break;
		case 2: outName.FromChar("helmikuu"); break;
		case 3: outName.FromChar("maaliskuu"); break;
		case 4: outName.FromChar("huhtikuu"); break;
		case 5: outName.FromChar("toukokuu"); break;
		case 6: outName.FromChar("kes\xe4kuu"); break;
		case 7: outName.FromChar("hein\xe4kuu"); break;
		case 8: outName.FromChar("elokuu"); break;
		case 9: outName.FromChar("syyskuu"); break;
		case 10: outName.FromChar("lokakuu"); break;
		case 11: outName.FromChar("marraskuu"); break;
		case 12: outName.FromChar("joulukuu"); break;
		}
		break;

	case kGZLanguage_Hebrew:
		switch (month)
		{
		case 1: outName.FromChar("January"); break;
		case 2: outName.FromChar("February"); break;
		case 3: outName.FromChar("March"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("June"); break;
		case 7: outName.FromChar("July"); break;
		case 8: outName.FromChar("August"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("October"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_Russian:
		switch (month)
		{
		case 1: outName.FromChar("January"); break;
		case 2: outName.FromChar("February"); break;
		case 3: outName.FromChar("March"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("June"); break;
		case 7: outName.FromChar("July"); break;
		case 8: outName.FromChar("August"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("October"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_Portuguese:
	case kGZLanguage_Portuguese_Brazilian:
		switch (month)
		{
		case 1: outName.FromChar("Janeiro"); break;
		case 2: outName.FromChar("Fevereiro"); break;
		case 3: outName.FromChar("Mar\xe7o"); break;
		case 4: outName.FromChar("Abril"); break;
		case 5: outName.FromChar("Maio"); break;
		case 6: outName.FromChar("Junho"); break;
		case 7: outName.FromChar("Julho"); break;
		case 8: outName.FromChar("Agosto"); break;
		case 9: outName.FromChar("Setembro"); break;
		case 10: outName.FromChar("Outubro"); break;
		case 11: outName.FromChar("Novembro"); break;
		case 12: outName.FromChar("Dezembro"); break;
		}
		break;

	case kGZLanguage_Japanese:
		switch (month)
		{
		case 1: outName.FromChar("1 \xe6\x9c\x88"); break;
		case 3: outName.FromChar("3 \xe6\x9c\x88"); break;
		case 4: outName.FromChar("4 \xe6\x9c\x88"); break;
		case 5: outName.FromChar("5 \xe6\x9c\x88"); break;
		case 6: outName.FromChar("6 \xe6\x9c\x88"); break;
		case 7: outName.FromChar("7 \xe6\x9c\x88"); break;
		case 8: outName.FromChar("8 \xe6\x9c\x88"); break;
		case 9: outName.FromChar("9 \xe6\x9c\x88"); break;
		case 10: outName.FromChar("10 \xe6\x9c\x88"); break;
		case 11: outName.FromChar("11 \xe6\x9c\x88"); break;
		case 12: outName.FromChar("12 \xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Polish:
		switch (month)
		{
		case 1: outName.FromChar("Stycze\xf1"); break;
		case 2: outName.FromChar("Luty"); break;
		case 3: outName.FromChar("Marzec"); break;
		case 4: outName.FromChar("Kwiecie\xf1"); break;
		case 5: outName.FromChar("Maj"); break;
		case 6: outName.FromChar("Czerwiec"); break;
		case 7: outName.FromChar("Lipiec"); break;
		case 8: outName.FromChar("Sierpie\xf1"); break;
		case 9: outName.FromChar("Wrzesie\xf1"); break;
		case 10: outName.FromChar("Pa\xc2\x9f" "dziernik"); break;
		case 11: outName.FromChar("Listopad"); break;
		case 12: outName.FromChar("Grudzie\xf1"); break;
		}
		break;

	case kGZLanguage_Chinese_Simplified:
		switch (month)
		{
		case 1: outName.FromChar("1\xe6\x9c\x88"); break;
		case 2: outName.FromChar("2\xe6\x9c\x88"); break;
		case 3: outName.FromChar("3\xe6\x9c\x88"); break;
		case 4: outName.FromChar("4\xe6\x9c\x88"); break;
		case 5: outName.FromChar("5\xe6\x9c\x88"); break;
		case 6: outName.FromChar("6\xe6\x9c\x88"); break;
		case 7: outName.FromChar("7\xe6\x9c\x88"); break;
		case 8: outName.FromChar("8\xe6\x9c\x88"); break;
		case 9: outName.FromChar("9\xe6\x9c\x88"); break;
		case 10: outName.FromChar("10\xe6\x9c\x88"); break;
		case 11: outName.FromChar("11\xe6\x9c\x88"); break;
		case 12: outName.FromChar("12\xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Chinese_Traditional:
		switch (month)
		{
		case 1: outName.FromChar("1\xe6\x9c\x88"); break;
		case 2: outName.FromChar("2\xe6\x9c\x88"); break;
		case 3: outName.FromChar("3\xe6\x9c\x88"); break;
		case 4: outName.FromChar("4\xe6\x9c\x88"); break;
		case 5: outName.FromChar("5\xe6\x9c\x88"); break;
		case 6: outName.FromChar("6\xe6\x9c\x88"); break;
		case 7: outName.FromChar("7\xe6\x9c\x88"); break;
		case 8: outName.FromChar("8\xe6\x9c\x88"); break;
		case 9: outName.FromChar("9\xe6\x9c\x88"); break;
		case 10: outName.FromChar("10\xe6\x9c\x88"); break;
		case 11: outName.FromChar("11\xe6\x9c\x88"); break;
		case 12: outName.FromChar("12\xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Thai:
		switch (month)
		{
		case 1: outName.FromChar("January"); break;
		case 2: outName.FromChar("February"); break;
		case 3: outName.FromChar("March"); break;
		case 4: outName.FromChar("April"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("June"); break;
		case 7: outName.FromChar("July"); break;
		case 8: outName.FromChar("August"); break;
		case 9: outName.FromChar("September"); break;
		case 10: outName.FromChar("October"); break;
		case 11: outName.FromChar("November"); break;
		case 12: outName.FromChar("December"); break;
		}
		break;

	case kGZLanguage_Korean:
		switch (month)
		{
		case 1: outName.FromChar("1\xec\x9b\x94"); break;
		case 2: outName.FromChar("\xec\x9d\xb4\xec\x9b\x94"); break;
		case 3: outName.FromChar("3\xec\x9b\x94"); break;
		case 4: outName.FromChar("4\xec\x9b\x94"); break;
		case 5: outName.FromChar("\xec\x98\xa4\xec\x9b\x94"); break;
		case 6: outName.FromChar("\xec\x9c\xa0\xec\x9b\x94"); break;
		case 7: outName.FromChar("\xec\xb9\xa0\xec\x9b\x94"); break;
		case 8: outName.FromChar("\xed\x8c\x94\xec\x9b\x94"); break;
		case 9: outName.FromChar("9\xec\x9b\x94"); break;
		case 10: outName.FromChar("10\xec\x9b\x94"); break;
		case 11: outName.FromChar("11\xec\x9b\x94"); break;
		case 12: outName.FromChar("\xec\x8b\xad\xec\x9d\xb4\xec\x9b\x94"); break;
		}
		break;
	}

	if (outName.Strlen() == 0)
	{
		outName.FromChar("Untranslated");
		translated = false;
	}

	ConvertUTF8StringToDestinationFormat(outName);
	return translated;
}

bool cRZLanguageUtility::GetAbbrMonthName(uint8_t month, cIGZString& outName) const
{
	bool translated = true;

	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_French:
		switch (month)
		{
		case 1: outName.FromChar("jan"); break;
		case 2: outName.FromChar("F\xe9v"); break;
		case 3: outName.FromChar("mar"); break;
		case 4: outName.FromChar("Avr"); break;
		case 5: outName.FromChar("mai"); break;
		case 6: outName.FromChar("juin"); break;
		case 7: outName.FromChar("juil"); break;
		case 8: outName.FromChar("Ao\xfbt"); break;
		case 9: outName.FromChar("sept"); break;
		case 10: outName.FromChar("oct"); break;
		case 11: outName.FromChar("nov"); break;
		case 12: outName.FromChar("D\xe9" "c"); break;
		}
		break;

	case kGZLanguage_German:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("M\xe4r"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("Mai"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Okt"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dez"); break;
		}
		break;

	case kGZLanguage_Italian:
		switch (month)
		{
		case 1: outName.FromChar("Gen"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("Mag"); break;
		case 6: outName.FromChar("Giu"); break;
		case 7: outName.FromChar("Lug"); break;
		case 8: outName.FromChar("Ago"); break;
		case 9: outName.FromChar("Set"); break;
		case 10: outName.FromChar("Ott"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dic"); break;
		}
		break;

	case kGZLanguage_Spanish:
		switch (month)
		{
		case 1: outName.FromChar("En"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Abr"); break;
		case 5: outName.FromChar("Mayo"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Ag"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dic"); break;
		}
		break;

	case kGZLanguage_Dutch:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mrt"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("Mei"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Okt"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Danish:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Swedish:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("Maj"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Okt"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Finnish:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Hebrew:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Russian:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Portuguese:
	case kGZLanguage_Portuguese_Brazilian:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Fev"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Abr"); break;
		case 5: outName.FromChar("Mai"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Ago"); break;
		case 9: outName.FromChar("Set"); break;
		case 10: outName.FromChar("Out"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dez"); break;
		}
		break;

	case kGZLanguage_Japanese:
		switch (month)
		{
		case 1: outName.FromChar("1 \xe6\x9c\x88"); break;
		case 2: outName.FromChar("2 \xe6\x9c\x88"); break;
		case 3: outName.FromChar("3 \xe6\x9c\x88"); break;
		case 4: outName.FromChar("4 \xe6\x9c\x88"); break;
		case 5: outName.FromChar("5 \xe6\x9c\x88"); break;
		case 6: outName.FromChar("6 \xe6\x9c\x88"); break;
		case 7: outName.FromChar("7 \xe6\x9c\x88"); break;
		case 8: outName.FromChar("8 \xe6\x9c\x88"); break;
		case 9: outName.FromChar("9 \xe6\x9c\x88"); break;
		case 10: outName.FromChar("10 \xe6\x9c\x88"); break;
		case 11: outName.FromChar("11 \xe6\x9c\x88"); break;
		case 12: outName.FromChar("12 \xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Polish:
		switch (month)
		{
		case 1: outName.FromChar("Sty"); break;
		case 2: outName.FromChar("Lut"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Kwi"); break;
		case 5: outName.FromChar("Maj"); break;
		case 6: outName.FromChar("Cze"); break;
		case 7: outName.FromChar("Lip"); break;
		case 8: outName.FromChar("Sie"); break;
		case 9: outName.FromChar("Wrz"); break;
		case 10: outName.FromChar("Pa\x9f"); break;
		case 11: outName.FromChar("Lis"); break;
		case 12: outName.FromChar("Gru"); break;
		}
		break;

	case kGZLanguage_Chinese_Simplified:
		switch (month)
		{
		case 1: outName.FromChar("1\xe6\x9c\x88"); break;
		case 2: outName.FromChar("2\xe6\x9c\x88"); break;
		case 3: outName.FromChar("3\xe6\x9c\x88"); break;
		case 4: outName.FromChar("4\xe6\x9c\x88"); break;
		case 5: outName.FromChar("5\xe6\x9c\x88"); break;
		case 6: outName.FromChar("6\xe6\x9c\x88"); break;
		case 7: outName.FromChar("7\xe6\x9c\x88"); break;
		case 8: outName.FromChar("8\xe6\x9c\x88"); break;
		case 9: outName.FromChar("9\xe6\x9c\x88"); break;
		case 10: outName.FromChar("10\xe6\x9c\x88"); break;
		case 11: outName.FromChar("11\xe6\x9c\x88"); break;
		case 12: outName.FromChar("12\xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Chinese_Traditional:
		switch (month)
		{
		case 1: outName.FromChar("1\xe6\x9c\x88"); break;
		case 2: outName.FromChar("2\xe6\x9c\x88"); break;
		case 3: outName.FromChar("3\xe6\x9c\x88"); break;
		case 4: outName.FromChar("4\xe6\x9c\x88"); break;
		case 5: outName.FromChar("5\xe6\x9c\x88"); break;
		case 6: outName.FromChar("6\xe6\x9c\x88"); break;
		case 7: outName.FromChar("7\xe6\x9c\x88"); break;
		case 8: outName.FromChar("8\xe6\x9c\x88"); break;
		case 9: outName.FromChar("9\xe6\x9c\x88"); break;
		case 10: outName.FromChar("10\xe6\x9c\x88"); break;
		case 11: outName.FromChar("11\xe6\x9c\x88"); break;
		case 12: outName.FromChar("12\xe6\x9c\x88"); break;
		}
		break;

	case kGZLanguage_Thai:
		switch (month)
		{
		case 1: outName.FromChar("Jan"); break;
		case 2: outName.FromChar("Feb"); break;
		case 3: outName.FromChar("Mar"); break;
		case 4: outName.FromChar("Apr"); break;
		case 5: outName.FromChar("May"); break;
		case 6: outName.FromChar("Jun"); break;
		case 7: outName.FromChar("Jul"); break;
		case 8: outName.FromChar("Aug"); break;
		case 9: outName.FromChar("Sep"); break;
		case 10: outName.FromChar("Oct"); break;
		case 11: outName.FromChar("Nov"); break;
		case 12: outName.FromChar("Dec"); break;
		}
		break;

	case kGZLanguage_Korean:
		switch (month)
		{
		case 1: outName.FromChar("1\xec\x9b\x94"); break;
		case 2: outName.FromChar("\xec\x9d\xb4\xec\x9b\x94"); break;
		case 3: outName.FromChar("3\xec\x9b\x94"); break;
		case 4: outName.FromChar("4\xec\x9b\x94"); break;
		case 5: outName.FromChar("\xec\x98\xa4\xec\x9b\x94"); break;
		case 6: outName.FromChar("\xec\x9c\xa0\xec\x9b\x94"); break;
		case 7: outName.FromChar("\xec\xb9\xa0\xec\x9b\x94"); break;
		case 8: outName.FromChar("\xed\x8c\x94\xec\x9b\x94"); break;
		case 9: outName.FromChar("9\xec\x9b\x94"); break;
		case 10: outName.FromChar("10\xec\x9b\x94"); break;
		case 11: outName.FromChar("11\xec\x9b\x94"); break;
		case 12: outName.FromChar("\xec\x8b\xad\xec\x9d\xb4\xec\x9b\x94"); break;
		}
		break;
	}

	if (outName.Strlen() == 0)
	{
		outName.FromChar("Untranslated");
		translated = false;
	}

	ConvertUTF8StringToDestinationFormat(outName);
	return translated;
}

bool cRZLanguageUtility::MakeTimeString(int32_t hour, int32_t minute, int32_t second, cIGZString& outTime) const
{
	// TODO
	return false;
}

bool cRZLanguageUtility::MakeDateString(int32_t day, int32_t month, uint32_t year, cIGZString& outDate, GZDateFormat format) const
{
	// TODO
	return false;
}

bool cRZLanguageUtility::GetCurrencySymbol(cIGZString& outSymbol) const
{
	bool result = true;

	if (mUseLocalCurrency)
	{
		switch (mLanguage)
		{
		case kGZLanguage_English:
		case kGZLanguage_Chinese_Simplified:
		case kGZLanguage_Chinese_Traditional:
			outSymbol.FromChar("$");
			break;

		case kGZLanguage_English_UK:
			outSymbol.FromChar("\xc2\xa3");
			break;

		case kGZLanguage_French:
		case kGZLanguage_German:
		case kGZLanguage_Italian:
		case kGZLanguage_Spanish:
		case kGZLanguage_Dutch:
		case kGZLanguage_Danish:
		case kGZLanguage_Norwegian:
		case kGZLanguage_Finnish:
		case kGZLanguage_Portuguese:
		case kGZLanguage_Greek:
		case kGZLanguage_French_Belgian:
			outSymbol.FromChar("\xe2\x82\xac");
			break;

		case kGZLanguage_Swedish:
		case kGZLanguage_Icelandic:
			outSymbol.FromChar("kr");
			break;

		case kGZLanguage_Hebrew:
			outSymbol.FromChar("\xe2\x82\xaa");
			break;

		case kGZLanguage_Russian:
			outSymbol.FromChar("\xd0\xb1");
			break;

		case kGZLanguage_Japanese:
			outSymbol.FromChar("\xc2\xa5");
			break;

		case kGZLanguage_Polish:
			outSymbol.FromChar("\xc5\x82");
			break;

		case kGZLanguage_Thai:
			outSymbol.FromChar("\xe0\xb8\xbf");
			break;

		case kGZLanguage_Korean:
			outSymbol.FromChar("\\");
			break;

		case kGZLanguage_Hindi:
		case kGZLanguage_Arabic:
		case kGZLanguage_Bulgarian:
		case kGZLanguage_Cyrillic:
		case kGZLanguage_Ukrainian:
		case kGZLanguage_Czech:
			outSymbol.FromChar("K\xc4\x8d\x7b");
			break;

		case kGZLanguage_Hungarian:
			outSymbol.FromChar("FT");
			break;

		case kGZLanguage_Romanian:
			outSymbol.FromChar("Lei");
			break;

		case kGZLanguage_Albanian:
			outSymbol.FromChar("Lek");
			break;

		case kGZLanguage_Turkish:
			outSymbol.FromChar("TL");
			break;

		case kGZLanguage_Portuguese_Brazilian:
			outSymbol.FromChar("Esc.");
			break;

		default:
			break;
		}
	}
	else
	{
		outSymbol.FromChar("$");
	}

	if (outSymbol.Strlen() == 0)
	{
		outSymbol.FromChar("Untranslated");
		result = false;
	}

	ConvertUTF8StringToDestinationFormat(outSymbol);
	return result;
}

bool cRZLanguageUtility::DoesCurrencySymbolPrecedeAmount() const
{
	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
	case kGZLanguage_Italian:
	case kGZLanguage_Dutch:
	case kGZLanguage_Danish:
	case kGZLanguage_Norwegian:
	case kGZLanguage_Hebrew:
	case kGZLanguage_Japanese:
	case kGZLanguage_Chinese_Simplified:
	case kGZLanguage_Chinese_Traditional:
	case kGZLanguage_Thai:
	case kGZLanguage_Korean:
	case kGZLanguage_Arabic:
	case kGZLanguage_Bulgarian:
		return true;

	default:
		return false;
	}
}

bool cRZLanguageUtility::IsSpaceBetweenCurrencySymbolAndAmount() const
{
	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
	case kGZLanguage_Norwegian:
	case kGZLanguage_Hebrew:
	case kGZLanguage_Japanese:
	case kGZLanguage_Chinese_Simplified:
	case kGZLanguage_Chinese_Traditional:
	case kGZLanguage_Thai:
	case kGZLanguage_Korean:
		return false;

	default:
		return true;
	}
}

bool cRZLanguageUtility::GetDecimalSeparator(cIGZString& outSeparator) const
{
	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
	case kGZLanguage_Hebrew:
	case kGZLanguage_Japanese:
	case kGZLanguage_Chinese_Simplified:
	case kGZLanguage_Chinese_Traditional:
	case kGZLanguage_Thai:
	case kGZLanguage_Korean:
		outSeparator.FromChar(",");
		return true;

	case kGZLanguage_French:
	case kGZLanguage_German:
	case kGZLanguage_Italian:
	case kGZLanguage_Spanish:
	case kGZLanguage_Dutch:
	case kGZLanguage_Danish:
	case kGZLanguage_Swedish:
	case kGZLanguage_Norwegian:
	case kGZLanguage_Finnish:
	case kGZLanguage_Russian:
	case kGZLanguage_Portuguese:
	case kGZLanguage_Polish:
	case kGZLanguage_Portuguese_Brazilian:
		outSeparator.FromChar(".");
		return true;

	default:
		outSeparator.FromChar(",");
		return false;
	}
}

bool cRZLanguageUtility::GetThousandSeparator(cIGZString& outSeparator) const
{
	switch (mLanguage)
	{
	case kGZLanguage_English:
	case kGZLanguage_English_UK:
	case kGZLanguage_Hebrew:
	case kGZLanguage_Japanese:
	case kGZLanguage_Chinese_Simplified:
	case kGZLanguage_Chinese_Traditional:
	case kGZLanguage_Thai:
	case kGZLanguage_Korean:
		outSeparator.FromChar(".");
		return true;

	case kGZLanguage_French:
	case kGZLanguage_German:
	case kGZLanguage_Italian:
	case kGZLanguage_Spanish:
	case kGZLanguage_Dutch:
	case kGZLanguage_Danish:
	case kGZLanguage_Swedish:
	case kGZLanguage_Russian:
	case kGZLanguage_Portuguese:
	case kGZLanguage_Portuguese_Brazilian:
		outSeparator.FromChar(",");
		return true;

	case kGZLanguage_Norwegian:
	case kGZLanguage_Finnish:
	case kGZLanguage_Polish:
		outSeparator.FromChar(mNonBreakingSpace.Data(), mNonBreakingSpace.Strlen());
		return true;

	default:
		outSeparator.FromChar(".");
		return false;
	}
}

bool cRZLanguageUtility::MakeMoneyString(int64_t amount, cIGZString& outMoney, cIGZString const* currencySymbolOverride) const
{
	cRZString rawDigits;
	cRZString decimalSeparator(".");
	cRZString numericPart;
	cRZString money;
	cRZString symbol;
	cRZString minusString("-");
	cRZString result;

	bool negative = (amount < 0);
	if (negative)
	{
		amount = -amount;
	}

	// Put together the place-separated number part of the string (e.g. "1,000,000")
	rawDigits.Sprintf("%lld", amount);
	GetDecimalSeparator(decimalSeparator);

	int32_t const digitCount = rawDigits.Strlen();
	int32_t i = digitCount;

	while (i > 3)
	{
		int32_t decimalPlace = i % 3;
		if (decimalPlace == 0)
		{
			decimalPlace = 3;
		}

		numericPart += rawDigits.Mid(digitCount - i, decimalPlace);
		numericPart += decimalSeparator;

		i -= decimalPlace;
	}

	numericPart += rawDigits.Right(i);

	// Add the currency symbol and (optional) space
	if (DoesCurrencySymbolPrecedeAmount())
	{
		if (currencySymbolOverride != NULL)
		{
			money = *currencySymbolOverride;
		}
		else
		{
			GetCurrencySymbol(money);
		}

		if (IsSpaceBetweenCurrencySymbolAndAmount())
		{
			money += mNonBreakingSpace;
		}

		money += numericPart;
	}
	else
	{
		money = numericPart;
		if (IsSpaceBetweenCurrencySymbolAndAmount())
		{
			money += mNonBreakingSpace;
		}

		if (currencySymbolOverride != NULL)
		{
			symbol = *currencySymbolOverride;
		}
		else
		{
			GetCurrencySymbol(symbol);
		}

		money += symbol;
	}

	if (negative)
	{
		switch (mLanguage)
		{
		case kGZLanguage_Danish:
		case kGZLanguage_Hebrew:
			if (currencySymbolOverride != NULL)
			{
				result = *currencySymbolOverride;
			}
			else
			{
				GetCurrencySymbol(result);
			}

			result += mNonBreakingSpace;
			result += minusString;
			result += numericPart;
			break;

		case kGZLanguage_Norwegian:
			if (currencySymbolOverride != NULL)
			{
				result = *currencySymbolOverride;
			}
			else
			{
				GetCurrencySymbol(result);
			}

			result += minusString;
			result += numericPart;
			break;

		default:
			result = minusString + money;
			break;
		}
	}
	else
	{
		result = money;
	}

	outMoney.FromChar(result.Data());
	return true;
}

bool cRZLanguageUtility::MakeFormattedMoneyString(double amount, cIGZString& outMoney, cIGZString& outSymbol, cIGZString const* currencySymbolOverride) const
{
	// TODO
	return false;
}

bool cRZLanguageUtility::MakeNumberString(int64_t value, cIGZString& outNumber) const
{
	// TODO
	return false;
}

bool cRZLanguageUtility::MakeFormattedNumberString(double value, cIGZString& outNumber, cIGZString const& format) const
{
	outNumber.Sprintf(format.ToChar(), value);
	return true;
}

void cRZLanguageUtility::ConvertToLowerCase(cIGZString const& src, cIGZString& dest) const
{
	cRZString convertedSrc;
	ConvertStringEncoding(src, convertedSrc, GZ_CODEPAGE_SYSTEM);

	CharLowerBuff(const_cast<char*>(convertedSrc.Data()), dest.Strlen());
	ConvertStringEncoding(convertedSrc, GZ_CODEPAGE_SYSTEM, dest);
}

void cRZLanguageUtility::ConvertToUpperCase(cIGZString const& src, cIGZString& dest) const
{
	cRZString convertedSrc;
	ConvertStringEncoding(src, convertedSrc, GZ_CODEPAGE_SYSTEM);

	CharUpperBuff(const_cast<char*>(convertedSrc.Data()), dest.Strlen());
	ConvertStringEncoding(convertedSrc, GZ_CODEPAGE_SYSTEM, dest);
}

int32_t cRZLanguageUtility::CompareStrings(cIGZString const& first, cIGZString const& second, bool caseSensitive) const
{
	cRZString firstConverted;
	cRZString secondConverted;

	ConvertStringEncoding(first, firstConverted, GZ_CODEPAGE_SYSTEM);
	ConvertStringEncoding(second, secondConverted, GZ_CODEPAGE_SYSTEM);

	int32_t result = CompareString(
		LOCALE_USER_DEFAULT,
		(caseSensitive ? 0 : NORM_IGNORECASE),
		firstConverted.Data(),
		firstConverted.Strlen(),
		secondConverted.Data(),
		secondConverted.Strlen());

	// WinAPI CompareString uses 1 for LESS_THAN, 2 for EQUAL, and 3 for GREATER_THAN.
	// This API is expecting strcmp-style values of (-1: less, 0: equal, 1: greater)
	if (result != 0)
	{
		result -= 2;
	}

	return result;
}

int32_t cRZLanguageUtility::GetCharacterSetFromLanguage() const
{
	if (DoesLanguageUseCharacterSet(kGZCharSet_ANSI))
	{
		return kGZCharSet_ANSI;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_CentralEurope))
	{
		return kGZCharSet_CentralEurope;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Thai))
	{
		return kGZCharSet_Thai;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_JapaneseShiftJIS))
	{
		return kGZCharSet_JapaneseShiftJIS;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_SimplifiedChinese))
	{
		return kGZCharSet_SimplifiedChinese;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_TraditionalChinese))
	{
		return kGZCharSet_TraditionalChinese;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Korean))
	{
		return kGZCharSet_Korean;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Cyrillic))
	{
		return kGZCharSet_Cyrillic;
	}

	// TODO: yes, the Windows version of the game really seems to do this twice.
	if (DoesLanguageUseCharacterSet(kGZCharSet_Cyrillic))
	{
		return kGZCharSet_Cyrillic;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Greek))
	{
		return kGZCharSet_Greek;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Turkish))
	{
		return kGZCharSet_Turkish;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Hebrew))
	{
		return kGZCharSet_Hebrew;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Arabic))
	{
		return kGZCharSet_Arabic;
	}

	if (DoesLanguageUseCharacterSet(kGZCharSet_Baltic))
	{
		return kGZCharSet_Baltic;
	}

	// TODO: same deal here as the previous TODO
	return (DoesLanguageUseCharacterSet(kGZCharSet_Baltic) ? kGZCharSet_Baltic : kGZCharSet_ANSI);
}

bool cRZLanguageUtility::DoesLanguageUseCharacterSet(GZCharacterSet characterSet) const
{
	if (characterSet == kGZCharSet_ANSI)
	{
		return mLanguage == kGZLanguage_English
			|| mLanguage == kGZLanguage_English_UK
			|| mLanguage == kGZLanguage_French
			|| mLanguage == kGZLanguage_German
			|| mLanguage == kGZLanguage_Italian
			|| mLanguage == kGZLanguage_Danish
			|| mLanguage == kGZLanguage_Swedish
			|| mLanguage == kGZLanguage_Dutch
			|| mLanguage == kGZLanguage_Norwegian
			|| mLanguage == kGZLanguage_Portuguese
			|| mLanguage == kGZLanguage_Portuguese_Brazilian
			|| mLanguage == kGZLanguage_Finnish
			|| mLanguage == kGZLanguage_Spanish
			|| mLanguage == kGZLanguage_Hungarian
			|| mLanguage == kGZLanguage_Icelandic
			|| mLanguage == kGZLanguage_Romanian
			|| mLanguage == kGZLanguage_Latin
			|| mLanguage == kGZLanguage_Spanish_Mexican
			|| mLanguage == kGZLanguage_French_Swiss
			|| mLanguage == kGZLanguage_French_Canadian
			|| mLanguage == kGZLanguage_French_Belgian
			|| mLanguage == kGZLanguage_German_Swiss
			|| mLanguage == kGZLanguage_Italian_Swiss
			|| mLanguage == kGZLanguage_Dutch_Belgian;
	}
	else if (characterSet == kGZCharSet_CentralEurope)
	{
		return mLanguage == kGZLanguage_Polish
			|| mLanguage == kGZLanguage_Russian
			|| mLanguage == kGZLanguage_Albanian
			|| mLanguage == kGZLanguage_Czech
			|| mLanguage == kGZLanguage_Hungarian
			|| mLanguage == kGZLanguage_Romanian
			|| mLanguage == kGZLanguage_Slovak
			|| mLanguage == kGZLanguage_Bulgarian;
	}
	else if (characterSet == kGZCharSet_JapaneseShiftJIS)
	{
		return mLanguage == kGZLanguage_Japanese;
	}
	else if (characterSet == kGZCharSet_Korean)
	{
		return mLanguage == kGZLanguage_Korean;
	}
	else if (characterSet == kGZCharSet_SimplifiedChinese)
	{
		return mLanguage == kGZLanguage_Chinese_Simplified;
	}
	else if (characterSet == kGZCharSet_TraditionalChinese)
	{
		return mLanguage == kGZLanguage_Chinese_Traditional;
	}
	else if (characterSet == kGZCharSet_Thai)
	{
		return mLanguage == kGZLanguage_Thai;
	}
	else if (characterSet == kGZCharSet_Cyrillic)
	{
		return mLanguage == kGZLanguage_Russian
			|| mLanguage == kGZLanguage_Cyrillic
			|| mLanguage == kGZLanguage_Bulgarian;
	}
	else if (characterSet == kGZCharSet_Greek)
	{
		return mLanguage == kGZLanguage_Greek;
	}
	else if (characterSet == kGZCharSet_Turkish)
	{
		return mLanguage == kGZLanguage_Turkish;
	}
	else if (characterSet == kGZCharSet_Hebrew)
	{
		return mLanguage == kGZLanguage_Hebrew;
	}
	else if (characterSet == kGZCharSet_Arabic)
	{
		return mLanguage == kGZLanguage_Arabic;
	}
	else
	{
		return false;
	}
}

bool cRZLanguageUtility::GetLanguageRoadDrivingSide() const
{
	if (mLanguage == kGZLanguage_English_UK || mLanguage == kGZLanguage_Japanese)
	{
		return false;
	}

	return true;
}

bool cRZLanguageUtility::GetMeasurementSystem() const
{
	// *eagle screeching intensifies*
	return mCountry != kGZCountry_UnitedStates;
}

bool cRZLanguageUtility::AreFirstAndLastNamesReversed() const
{
	if (mLanguage == kGZLanguage_Japanese
		|| mLanguage == kGZLanguage_Chinese_Simplified
		|| mLanguage == kGZLanguage_Chinese_Traditional
		|| mLanguage == kGZLanguage_Korean)
	{
		return true;
	}

	return false;
}

bool cRZLanguageUtility::DoesLanguageUseInputMethodEditing() const
{
	return mLanguage == kGZLanguage_Japanese
		|| mLanguage == kGZLanguage_Chinese_Simplified
		|| mLanguage == kGZLanguage_Chinese_Traditional
		|| mLanguage == kGZLanguage_Korean;
}

bool cRZLanguageUtility::DoesLanguageUseMultiByteCharacters() const
{
	switch (GetCurrentFrameworkCodePage())
	{
	case 0:
		return false;

	case GZ_CODEPAGE_UTF8:
		return mLanguage != kGZLanguage_English && mLanguage != kGZLanguage_English_UK;

	case GZ_CODEPAGE_UTF16:
		return false;

	default:
		return mLanguage == kGZLanguage_Japanese
			|| mLanguage == kGZLanguage_Chinese_Simplified
			|| mLanguage == kGZLanguage_Chinese_Traditional
			|| mLanguage == kGZLanguage_Thai
			|| mLanguage == kGZLanguage_Korean;
	}
}

void cRZLanguageUtility::SetLanguage(GZLanguage language)
{
	mLanguage = language;
}

void cRZLanguageUtility::SetCountry(uint32_t country)
{
	mCountry = country;
}