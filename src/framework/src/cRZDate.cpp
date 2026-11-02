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

#include <cstring>
#include <ctime>
#include "cGZDate.h"
#include "cRZDate.h"
#include "cRZString.h"
#include "cRZTime.h"

cRZDate cRZDate::mRefDate = cRZDate(0, 0);
cRZDate cRZDate::MaxDate = cRZDate(49709, 0);

cRZDate::cRZDate()
{
	time_t currentTimestamp = time(NULL);
	struct tm* localTime = localtime(&currentTimestamp);

	mDateCode = Jday(localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year + 1900);
}

cRZDate::cRZDate(cIGZDate const& date) :
	mDateCode(date.DayNumber())
{
}

cRZDate::cRZDate(cGZDate const& date) :
	mDateCode(date.DayNumber())
{
}

cRZDate::cRZDate(uint32_t month, uint32_t day, uint32_t year)
{
	mDateCode = Jday(month, day, year);
}

uint32_t cRZDate::DayOfMonth(void) const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return day;
}

uint32_t cRZDate::DayOfYear(void) const
{
	return mDateCode - Jday(12, 31, Year() - 1);
}

uint32_t cRZDate::FirstDayOfMonth(uint32_t month) const
{
	static const uint32_t kFirstDayOfEachMonth[] =
	{
		-1, 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
	};

	if (!MonthIsValid(month))
	{
		return 0;
	}

	uint32_t result = kFirstDayOfEachMonth[month];
	if (month > 2 && IsLeapYear(Year()))
	{
		result++;
	}

	return result;
}

void cRZDate::Mdy(uint32_t& month, uint32_t& day, uint32_t& year) const
{
	// Convert our Julian date code to a Gregorian month/day/year format
	// See "algorithm 199" by Robert G. Tantzen in ACM Communications, Vol. 6., Issue 8

	// 
	const uint32_t kDaysFromJulianToGregorianBase = 1721119;
	uint32_t gregorianDateCode = mDateCode - kDaysFromJulianToGregorianBase;

	const uint32_t kDaysInGregorianCycle = 146097;
	year = ((gregorianDateCode * 4) - 1) / kDaysInGregorianCycle;

	const uint32_t kDaysPerShortLeapCycle = (365 * 4) + 1;

	uint32_t x = ((((gregorianDateCode * 4) - 1) - (year * kDaysInGregorianCycle)) / 4) * 4;
	uint32_t y = (x + 3) / kDaysPerShortLeapCycle;
	uint32_t z = ((x - (y * kDaysPerShortLeapCycle)) + 7) / 4;

	const uint32_t kDaysPerFiveMonths = 153;
	uint32_t m = ((z * 5) - 3) / kDaysPerFiveMonths;

	month = m;
	day = ((z * 5) - (m * kDaysPerFiveMonths) + 2) / 5;
	year = year * 100 + y;

	if (month < 10)
	{
		month += 3;
	}
	else
	{
		month -= 9;
		year++;
	}
}

uint32_t cRZDate::Month(void) const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return month;
}

uint32_t cRZDate::Year(void) const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return year;
}

void cRZDate::Set(uint32_t month, uint32_t day, uint32_t year)
{
	mDateCode = Jday(month, day, year);
}

void cRZDate::Set(uint32_t day, uint32_t year)
{
	if (year != 0)
	{
		mDateCode = Jday(12, 31, year - 1) + day;
	}
	else
	{
		mDateCode = kStartOfYear1901 + day;
	}
}

void cRZDate::Set(char const* monthAbbr, uint32_t day, uint32_t year)
{
	static char const* kMonthAbbrs[] =
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};

	uint32_t i = 1;
	uint32_t month = 1;

	for (int i = 0; i < 12; i++)
	{
		if (strnicmp(kMonthAbbrs[i], monthAbbr, 3) == 0)
		{
			month = i + 1;
		}
	}

	mDateCode = Jday(month, day, year);
}

void cRZDate::Set(cRZTime const& time)
{
	uint32_t newDateCode = 0;
	if (time.Timestamp() > 0)
	{
		newDateCode = (time.GMT() / 86400) + kStartOfYear1901;
	}

	mDateCode = newDateCode;
}

bool cRZDate::Between(cRZDate const& left, cRZDate const& right) const
{
	if (left.DateCode() <= right.DateCode())
	{
		if (mDateCode >= left.DateCode() && mDateCode <= right.DateCode())
		{
			return true;
		}
	}
	else
	{
		if (mDateCode >= right.DateCode() && mDateCode <= left.DateCode())
		{
			return true;
		}
	}

	return false;
}

int32_t cRZDate::CompareTo(cRZDate const& other) const
{
	if (mDateCode < other.DateCode())
	{
		return -1;
	}
	else if (mDateCode > other.DateCode())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

cRZDate cRZDate::Max(cRZDate const& other) const
{
	cRZDate const* max = this;
	if (other.DateCode() > mDateCode)
	{
		max = &other;
	}

	return *max;
}

cRZDate cRZDate::Min(cRZDate const& other) const
{
	cRZDate const* max = this;
	if (other.DateCode() < mDateCode)
	{
		max = &other;
	}

	return *max;
}

cRZDate cRZDate::Previous(uint32_t dayOfWeek) const
{
	uint32_t target = dayOfWeek - 1;
	uint32_t current = WeekDay() - 1;
	uint32_t code = mDateCode;

	if (target > current)
	{
		current = current + (7 - target);
	}
	else
	{
		current = current - target;
	}

	return cRZDate(code - current);
}

void cRZDate::NameOfDayString(cRZString& out, uint32_t format) const
{
	// TODO
}

void cRZDate::NameOfMonthString(cRZString& out, uint32_t format) const
{
	// TODO
}

void cRZDate::DateString(cRZString& out, uint32_t format) const
{
	if (format == 0)
	{
		format = 1;
	}

	switch (format)
	{
	case -1:
	case 0:
	case 1:
		out.Sprintf("%u/%u/%4u", Month(), DayOfMonth(), Year());
		break;

	case 2:
	case 5:
	case 6:
	case 13:
		out.Sprintf("%u/%u/%4u", DayOfMonth(), Month(), Year());
		break;

	case 3:
	case 4:
	case 10:
	case 11:
		out.Sprintf("%u.%u.%4u", DayOfMonth(), Month(), Year());
		break;

	case 7:
	case 8:
	case 14:
		out.Sprintf("%u-%u-%4u", DayOfMonth(), Month(), Year());
		break;

	case 9:
		out.Sprintf("%4u.%u.%u", Year(), Month(), DayOfMonth());
		break;

	case 15:
		out.Sprintf("%4u/%u/%u", Year(), Month(), DayOfMonth());
		break;

	default:
		out.Sprintf("%u/%u/%4u", DayOfMonth(), Month(), Year());
		break;
	}
}