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

#include <cstring>
#include <ctime>
#include "cGZDate.h"
#include "cRZDate.h"
#include "cRZString.h"

cRZDate::cRZDate()
{
	time_t currentTimestamp = time(NULL);
	struct tm* localTime = localtime(&currentTimestamp);

	dateCode = Jday(localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year + 1900);
}

cRZDate::cRZDate(cIGZDate const& date) :
	dateCode(date.DayNumber())
{
}

cRZDate::cRZDate(cGZDate const& date) :
	dateCode(date.DayNumber())
{
}

cRZDate::cRZDate(cRZDate const& date) :
	dateCode(date.dateCode)
{
}

cRZDate::cRZDate(uint32_t dayNumber) :
	dateCode(dayNumber)
{
}

cRZDate::cRZDate(uint32_t day, uint32_t year)
{
	static const uint32_t kStartOfYear1901 = 2415386;

	if (year == 0)
	{
		dateCode = kStartOfYear1901 + day;
	}
	else
	{
		dateCode = Jday(12, 31, year - 1) + day;
	}
}

cRZDate::cRZDate(uint32_t month, uint32_t day, uint32_t year)
{
	dateCode = Jday(month, day, year);
}

uint32_t cRZDate::DayOfMonth() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return day;
}

uint32_t cRZDate::DayOfYear() const
{
	uint32_t month, day, year;
	uint32_t date = dateCode;

	Mdy(month, day, year);
	uint32_t endOfPrevYear = Jday(12, 31, year - 1);

	return date - endOfPrevYear;
}

uint32_t cRZDate::FirstDayOfMonth(uint32_t month) const
{
	static uint32_t kFirstDayOfEachMonth[] =
	{
		-1, 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
	};

	uint32_t result = 0;
	if (month < 13 && month != 0 && (result = kFirstDayOfEachMonth[month]) && month > 2)
	{
		uint32_t month, day, year;
		Mdy(month, day, year);

		bool notLeapYear;
		if (year % 4 == 0 && year != ((year / 100) * 100))
		{
			notLeapYear = false;
		}
		else
		{
			notLeapYear = (year != ((year / 400) * 400));
		}

		result += !notLeapYear;
	}

	return result;
}

void cRZDate::Mdy(uint32_t& month, uint32_t& day, uint32_t& year) const
{
	uint32_t x, y, z;
	x = (dateCode * 4 - 6884477) / 146097;
	year = x;

	x = (-146097 * x) + 6884477 + ((dateCode * 4) + 3);
	z = x / 1461;
	y = (z * -1461) + 4 + (x / 4);
	x = ((y * 5) - 3) / 153;

	month = x;
	day = ((x * -153) + 2 + (y * 5)) / 5;
	year = year * 100 + z;
	
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

uint32_t cRZDate::Month() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return month;
}

uint32_t cRZDate::Year() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return year;
}

void cRZDate::Set(char const* monthAbbr, uint32_t day, uint32_t year)
{
	static char const* kMonthAbbrs[] =
	{
		NULL,
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};

	uint32_t i = 1;
	uint32_t month = 1;

	do
	{
		if (strnicmp(kMonthAbbrs[i], monthAbbr, 3) == 0)
		{
			month = i;
		}
	}
	while (++i != 13);

	dateCode = Jday(month, day, year);
}

void cRZDate::Set(uint32_t month, uint32_t day, uint32_t year)
{
	dateCode = Jday(month, day, year);
}

void cRZDate::Set(uint32_t newDateCode)
{
	dateCode = newDateCode;
}

bool cRZDate::DayWithinMonth(uint32_t month, uint32_t day, uint32_t year)
{
	static const uint32_t kDaysInMonth[] =
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
	};

	bool result = false;
	if (day != 0 && month < 13 && month != 0)
	{
		bool isLeapYear = false;
		if (year % 4 == 0 && year != ((year / 100) * 100))
		{
			isLeapYear = true;
		}
		else
		{
			isLeapYear = (year == ((year / 400) * 400));
		}

		result = (day <= ((month == 2 && isLeapYear) + (uint8_t)kDaysInMonth[month]));
	}

	return result;
}

uint32_t cRZDate::Jday(uint32_t month, uint32_t day, uint32_t year)
{
	uint32_t dateCode = 0;
	if (DayWithinMonth(month, day, year))
	{
		if (month < 3)
		{
			dateCode = month + 9;
			year = year - 1;
		}
		else
		{
			dateCode = month - 3;
		}

		dateCode = (((year % 100) * 1461) / 4) + 1721119 + day + ((dateCode * 153) + 2) / 5 + (((year / 100) * 146097) / 4);
	}

	return dateCode;
}

void cRZDate::NameOfDayString(cRZString& out, uint32_t format) const
{
}

void cRZDate::NameOfMonthString(cRZString& out, uint32_t format) const
{
}

void cRZDate::DateString(cRZString& out, uint32_t format) const
{
	uint32_t month, day, year;
	if (format == 0)
	{
		format = 1;
	}

	switch (format)
	{
	case -1:
	case 0:
	case 1:
		Mdy(month, day, year);
		out.Sprintf("%u/%u/%4u", month, day, year);
		break;

	case 2:
	case 5:
	case 6:
	case 13:
		Mdy(month, day, year);
		out.Sprintf("%u/%u/%4u", day, month, year);
		break;

	case 3:
	case 4:
	case 10:
	case 11:
		Mdy(month, day, year);
		out.Sprintf("%u.%u.%4u", day, month, year);
		break;

	case 7:
	case 8:
	case 14:
		Mdy(month, day, year);
		out.Sprintf("%u-%u-%4u", day, month, year);
		break;

	case 9:
		Mdy(month, day, year);
		out.Sprintf("%4u.%u.%u", year, month, day);
		break;

	case 15:
		Mdy(month, day, year);
		out.Sprintf("%4u/%u/%u", year, month, day);
		break;

	default:
		Mdy(month, day, year);
		out.Sprintf("%u/%u/%4u", day, month, year);
		break;
	}
}