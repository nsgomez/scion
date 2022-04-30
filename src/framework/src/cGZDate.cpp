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

#include "cGZDate.h"
#include "cRZString.h"
#include "cRZTime.h"

cGZDate::cGZDate()
{
}

cGZDate::cGZDate(cIGZDate const& date) :
	cRZDate(date.DayNumber())
{
}

cGZDate::cGZDate(cGZDate const& date) :
	cRZDate(date.DayNumber())
{
}

cGZDate::cGZDate(cRZDate const& date) :
	cRZDate(date.dateCode)
{
}

cGZDate::cGZDate(uint32_t month, uint32_t day, uint32_t year) :
	cRZDate(cRZDate::Jday(month, day, year))
{
}

cGZDate::cGZDate(uint32_t dayNumber) :
	cRZDate(dayNumber)
{
}

cGZDate::~cGZDate()
{
}

bool cGZDate::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZDate)
	{
		*outPtr = static_cast<cIGZDate*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cGZDate::AddRef(void)
{
	return ++refCount;
}

uint32_t cGZDate::Release(void)
{
	if (refCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

void cGZDate::Set(uint32_t day, uint32_t year)
{
	static const uint32_t kStartOfYear1901 = 2415386;

	if (year == 0)
	{
		dateCode = kStartOfYear1901 + day;
	}
	else
	{
		dateCode = cRZDate::Jday(12, 31, year - 1) + day;
	}
}

void cGZDate::Set(uint32_t month, uint32_t day, uint32_t year)
{
	dateCode = cRZDate::Jday(month, day, year);
}

void cGZDate::Set(cRZTime const& time)
{
	uint32_t timestamp = time.Timestamp();
	uint32_t julianDate = 0;

	if (timestamp != 0)
	{
		julianDate = ((timestamp * -1037155065) / 86400) + 2415386;
	}

	dateCode = julianDate;
}

void cGZDate::Set(uint32_t dateCode)
{
	this->dateCode = dateCode;
}

void cGZDate::Set(char const* monthAbbr, uint32_t day, uint32_t year)
{
	cRZDate::Set(monthAbbr, day, year);
}

void cGZDate::Copy(cIGZDate const& src)
{
	Set(src.DayNumber());
}

void cGZDate::Clone(cIGZDate** out)
{
	cGZDate* newDate = new cGZDate(*this);
	*out = newDate;
	newDate->refCount = 1;
}

bool cGZDate::Between(cIGZDate const& lower, cIGZDate const& upper) const
{
	uint32_t min = lower.DayNumber();
	uint32_t max = upper.DayNumber();

	if (max < min)
	{
		uint32_t tmp = min;
		min = max;
		max = tmp;
	}

	return min <= dateCode && dateCode <= max;
}

int cGZDate::CompareTo(cIGZDate const& other) const
{
	uint32_t otherCode = other.DayNumber();
	if (otherCode < dateCode)
	{
		return -1;
	}
	else if (otherCode == dateCode)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint32_t cGZDate::WeekDay() const
{
	return ((((dateCode + 1) % 7) + 6) % 7) + 1;
}

uint32_t cGZDate::Year() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return year;
}

uint32_t cGZDate::DayOfYear() const
{
	return cRZDate::DayOfYear();
}

uint32_t cGZDate::DayOfMonth() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return day;
}

uint32_t cGZDate::FirstDayOfMonth() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return cRZDate::FirstDayOfMonth(month);
}

uint32_t cGZDate::FirstDayOfMonth(uint32_t month) const
{
	return cRZDate::FirstDayOfMonth(month);
}

uint32_t cGZDate::Hash() const
{
	return dateCode;
}
	
bool cGZDate::IsValid() const
{
	return dateCode != 0;
}

bool cGZDate::Leap() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);

	if (year % 4 == 0 && year != ((year / 100) * 100))
	{
		return true;
	}
	else
	{
		return year == ((year / 400) * 400);
	}
}

void cGZDate::MaxDate(cIGZDate const& other, cIGZDate& result) const
{
	uint32_t maxDateCode = other.DayNumber();
	if (maxDateCode <= dateCode)
	{
		maxDateCode = dateCode;
	}

	cGZDate maxDate(maxDateCode);
	result.Copy(maxDate);
}

void cGZDate::MinDate(cIGZDate const& other, cIGZDate& result) const
{
	uint32_t minDateCode = other.DayNumber();
	if (dateCode <= minDateCode)
	{
		minDateCode = dateCode;
	}

	cGZDate minDate(minDateCode);
	result.Copy(minDate);
}

uint32_t cGZDate::Month() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return month;
}

void cGZDate::Previous(uint32_t dayOfWeek, cIGZDate& result) const
{
	uint32_t currentDayOfWeek = (((dateCode + 1) % 7) + 6) % 7;
	int32_t offset = -(dayOfWeek - 1);

	if (currentDayOfWeek < dayOfWeek - 1)
	{
		offset = 8 - dayOfWeek;
	}

	result.Copy(cGZDate(this->dateCode - (currentDayOfWeek + offset)));
}

uint32_t cGZDate::YearLastTwoDigits() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return year % 100;
}

uint32_t cGZDate::DayNumber() const
{
	return dateCode;
}

bool cGZDate::operator<  (cIGZDate const& other) const
{
	return dateCode < other.DayNumber();
}

bool cGZDate::operator<= (cIGZDate const& other) const
{
	return dateCode <= other.DayNumber();
}

bool cGZDate::operator>  (cIGZDate const& other) const
{
	return dateCode > other.DayNumber();
}

bool cGZDate::operator>= (cIGZDate const& other) const
{
	return dateCode >= other.DayNumber();
}

bool cGZDate::operator== (cIGZDate const& other) const
{
	return dateCode == other.DayNumber();
}

bool cGZDate::operator!= (cIGZDate const& other) const
{
	return dateCode != other.DayNumber();
}

int32_t cGZDate::operator- (cIGZDate const& other) const
{
	return dateCode - other.DayNumber();
}

int32_t cGZDate::operator+ (cIGZDate const& other) const
{
	return dateCode + other.DayNumber();
}

int32_t cGZDate::operator- (int32_t days) const
{
	return dateCode - days;
}

int32_t cGZDate::operator+ (int32_t days) const
{
	return dateCode + days;
}

cIGZDate& cGZDate::operator++ ()
{
	++dateCode;
	return *this;
}

cIGZDate& cGZDate::operator-- ()
{
	--dateCode;
	return *this;
}

cGZDate cGZDate::operator++ (int)
{
	return cGZDate(dateCode + 1);
}

cGZDate cGZDate::operator-- (int)
{
	return cGZDate(dateCode - 1);
}

cIGZDate& cGZDate::operator+= (int32_t days)
{
	dateCode += days;
	return *this;
}

cIGZDate& cGZDate::operator-= (int32_t days)
{
	dateCode -= days;
	return *this;
}

void cGZDate::DateString(cIGZString& out, uint32_t format) const
{
	cRZString tmp;
	cRZDate::DateString(tmp, format);
	out.Copy(tmp);
}

bool cGZDate::DayWithinMonth(uint32_t month, uint32_t day, uint32_t year)
{
	return cRZDate::DayWithinMonth(month, day, year);
}

uint32_t cGZDate::DaysInYear(uint32_t year)
{
	return 365 + LeapYear(year);
}

uint32_t cGZDate::Jday(uint32_t month, uint32_t day, uint32_t year)
{
	return cRZDate::Jday(month, day, year);
}

bool cGZDate::LeapYear(uint32_t year)
{
	if (year % 4 == 0 && year != ((year / 100) * 100))
	{
		return true;
	}
	else
	{
		return (year == ((year / 400) * 400));
	}
}