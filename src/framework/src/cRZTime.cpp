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

#include "cRZDate.h"
#include "cRZTime.h"

static const uint32_t kSecondsPerMinute = 60u;
static const uint32_t kSecondsPerHour = kSecondsPerMinute * 60u;
static const uint32_t kSecondsPerDay = kSecondsPerHour * 24u;
static const uint32_t kSunday = 7;

cRZTime::cRZTime()
{
	time_t rawTime;
	time(&rawTime);
	mTimestamp = (uint32_t)rawTime;
}

uint32_t cRZTime::Hour() const
{
	return GMT() % kSecondsPerDay / kSecondsPerHour;
}

uint32_t cRZTime::Minute() const
{
	return GMT() % kSecondsPerHour / kSecondsPerMinute;
}

bool cRZTime::IsDST() const
{
	if (!_daylight)
	{
		return 0;
	}

	uint32_t dayNumber = mTimestamp / kSecondsPerDay;
	uint32_t year = cRZDate(dayNumber, 0).Year();

	return mTimestamp >= DaylightSavingStart(year).Timestamp()
		&& mTimestamp < DaylightSavingEnd(year).Timestamp();
}

cRZTime cRZTime::DaylightSavingStart(uint32_t year)
{
	// Daylight savings starts in the United States at 2:00a.m. on the first Sunday in April,
	// so we start on this epoch and look back to the previous Sunday.
	cRZDate epoch(4, 8, year);

	if (year > 1986)
	{
		return cRZTime(
			(epoch.Previous(kSunday).DateCode() - cRZDate::mRefDate.DateCode()) * kSecondsPerDay
			+ _timezone
			+ (2 * kSecondsPerHour));
	}

	// There was an experiment with year-round daylight savings in 1974 and 1975.
	if (year == 1974)
	{
		return cRZTime(
			(cRZDate(1, 6, 1974).DateCode() - cRZDate::mRefDate.DateCode()) * kSecondsPerDay
			+ _timezone
			+ (2 * kSecondsPerHour));
	}

	if (year == 1975)
	{
		return cRZTime(
			(cRZDate(2, 23, 1975).DateCode() - cRZDate::mRefDate.DateCode()) * kSecondsPerDay
			+ _timezone
			+ (2 * kSecondsPerHour));
	}

	// BUG? TODO? Before 1986, DST is supposed to start on the *last* Sunday in April,
	// but the game seems to do another calculation for the first Sunday. (FUN_00417600)
	return cRZTime(
		(epoch.Previous(kSunday).DateCode() - cRZDate::mRefDate.DateCode()) * kSecondsPerDay
		+ _timezone
		+ (2 * kSecondsPerHour));
}

cRZTime cRZTime::DaylightSavingEnd(uint32_t year)
{
	// DST ends on the last Sunday on or before October 31 at 1:00a.m. standard time.
	cRZDate epoch(10, 31, year);

	return cRZTime(
		(epoch.Previous(kSunday).DateCode() - cRZDate::mRefDate.DateCode()) * kSecondsPerDay
		+ _timezone
		+ (1 * kSecondsPerHour));
}