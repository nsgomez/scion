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

#include "cRZDate.h"

uint32_t cRZDate::MonthIsValid(uint32_t month)
{
	uint32_t result = 1;
	if (month < 1 || month > 12)
	{
		result = 0;
	}

	return result;
}

uint32_t cRZDate::IsLeapYear(uint32_t year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

uint32_t cRZDate::DayWithinMonth(uint32_t month, uint32_t day, uint32_t year)
{
	if (day <= 0 || !MonthIsValid(month))
	{
		return 0;
	}

	static const uint32_t kDaysInMonth[] =
	{
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
	};

	uint32_t days = kDaysInMonth[month];
	if (IsLeapYear(year) && month == 2)
	{
		days ++;
	}

	return day <= days;
}

uint32_t cRZDate::DaysInYear(uint32_t year)
{
	return IsLeapYear(year) ? 366 : 365;
}

uint32_t cRZDate::Jday(uint32_t month, uint32_t day, uint32_t year)
{
	if (!DayWithinMonth(month, day, year))
	{
		return 0;
	}

	if (month > 2)
	{
		month -= 3;
	}
	else
	{
		month += 9;
		year--;
	}

	return (((year - ((year / 100) * 100) * 1461) / 4) + 1721119 + day
		+ (((month * 153) + 2) / 5)
		+ (((year / 100) * 146097) / 4));
}

uint32_t cRZDate::WeekDay(void) const
{
	return ((((mDateCode + 1) % 7) + 6) % 7) + 1;
}