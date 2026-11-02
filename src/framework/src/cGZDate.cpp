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
	cRZDate(date.mDateCode)
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
	return ++mRefCount;
}

uint32_t cGZDate::Release(void)
{
	if (mRefCount < 2)
	{
		delete this;
		return 0;
	}
	else
	{
		return --mRefCount;
	}
}

void cGZDate::Copy(cIGZDate const& src)
{
	Set(src.DayNumber());
}

void cGZDate::Clone(cIGZDate** out)
{
	cGZDate* newDate = new cGZDate(*this);
	*out = newDate;
	newDate->mRefCount = 1;
}

uint32_t cGZDate::Between(cIGZDate const& left, cIGZDate const& right) const
{
	return cRZDate::Between(left, right);
}

int32_t cGZDate::CompareTo(cIGZDate const& other) const
{
	cRZDate otherDate(other);
	return cRZDate::CompareTo(otherDate);
}

uint32_t cGZDate::FirstDayOfMonth() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return cRZDate::FirstDayOfMonth(month);
}

uint32_t cGZDate::Hash() const
{
	return mDateCode;
}
	
bool cGZDate::IsValid() const
{
	return mDateCode != 0;
}

uint32_t cGZDate::Leap() const
{
	return cRZDate::IsLeapYear(Year());
}

void cGZDate::MaxDate(cIGZDate const& other, cIGZDate& result) const
{
	uint32_t maxDateCode = other.DayNumber();
	if (maxDateCode <= mDateCode)
	{
		maxDateCode = mDateCode;
	}

	cGZDate maxDate(maxDateCode);
	result.Copy(maxDate);
}

void cGZDate::MinDate(cIGZDate const& other, cIGZDate& result) const
{
	uint32_t minDateCode = other.DayNumber();
	if (mDateCode <= minDateCode)
	{
		minDateCode = mDateCode;
	}

	cGZDate minDate(minDateCode);
	result.Copy(minDate);
}

void cGZDate::Previous(uint32_t dayOfWeek, cIGZDate& result) const
{
	uint32_t currentDayOfWeek = (((mDateCode + 1) % 7) + 6) % 7;
	int32_t offset = -(dayOfWeek - 1);

	if (currentDayOfWeek < dayOfWeek - 1)
	{
		offset = 8 - dayOfWeek;
	}

	result.Copy(cGZDate(this->mDateCode - (currentDayOfWeek + offset)));
}

uint32_t cGZDate::YearLastTwoDigits() const
{
	uint32_t month, day, year;
	Mdy(month, day, year);
	return year % 100;
}

uint32_t cGZDate::DayNumber() const
{
	return mDateCode;
}

bool cGZDate::operator<  (cIGZDate const& other) const
{
	return mDateCode < other.DayNumber();
}

bool cGZDate::operator<= (cIGZDate const& other) const
{
	return mDateCode <= other.DayNumber();
}

bool cGZDate::operator>  (cIGZDate const& other) const
{
	return mDateCode > other.DayNumber();
}

bool cGZDate::operator>= (cIGZDate const& other) const
{
	return mDateCode >= other.DayNumber();
}

bool cGZDate::operator== (cIGZDate const& other) const
{
	return mDateCode == other.DayNumber();
}

bool cGZDate::operator!= (cIGZDate const& other) const
{
	return mDateCode != other.DayNumber();
}

int32_t cGZDate::operator- (cIGZDate const& other) const
{
	return mDateCode - other.DayNumber();
}

int32_t cGZDate::operator+ (cIGZDate const& other) const
{
	return mDateCode + other.DayNumber();
}

int32_t cGZDate::operator- (int32_t days) const
{
	return mDateCode - days;
}

int32_t cGZDate::operator+ (int32_t days) const
{
	return mDateCode + days;
}

cIGZDate& cGZDate::operator++ ()
{
	++mDateCode;
	return *this;
}

cIGZDate& cGZDate::operator-- ()
{
	--mDateCode;
	return *this;
}

cGZDate cGZDate::operator++ (int)
{
	return cGZDate(mDateCode + 1);
}

cGZDate cGZDate::operator-- (int)
{
	return cGZDate(mDateCode - 1);
}

cIGZDate& cGZDate::operator+= (int32_t days)
{
	mDateCode += days;
	return *this;
}

cIGZDate& cGZDate::operator-= (int32_t days)
{
	mDateCode -= days;
	return *this;
}

void cGZDate::DateString(cIGZString& out, uint32_t format) const
{
	cRZString tmp;
	cRZDate::DateString(tmp, format);
	out.Copy(tmp);
}

uint32_t cGZDate::DayWithinMonth(uint32_t month, uint32_t day, uint32_t year)
{
	return cRZDate::DayWithinMonth(month, day, year);
}

uint32_t cGZDate::DaysInYear(uint32_t year)
{
	return cRZDate::DaysInYear(year);
}

uint32_t cGZDate::Jday(uint32_t month, uint32_t day, uint32_t year)
{
	return cRZDate::Jday(month, day, year);
}

uint32_t cGZDate::LeapYear(uint32_t year)
{
	return cRZDate::IsLeapYear(year);
}