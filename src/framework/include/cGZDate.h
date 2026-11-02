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

#pragma once
#include "cIGZDate.h"
#include "cIGZUnknown.h"
#include "cRZDate.h"

class cGZDate : public cRZDate, public cIGZDate
{
public:
	cGZDate();
	cGZDate(cIGZDate const& date);
	cGZDate(cGZDate const& date);
	cGZDate(cRZDate const& date);
	cGZDate(uint32_t month, uint32_t day, uint32_t year);
	cGZDate(uint32_t dateCode);
	virtual ~cGZDate() { }

public:
	virtual cGZDate operator++ (int); // post-increment
	virtual cGZDate operator-- (int); // post-increment

public:
	virtual void Set(uint32_t day, uint32_t year) { cRZDate::Set(day, year); }
	virtual void Set(uint32_t month, uint32_t day, uint32_t year) { cRZDate::Set(month, day, year); }
	virtual void Set(cRZTime const& time) { cRZDate::Set(time); }
	virtual void Set(uint32_t dateCode) { cRZDate::Set(dateCode); }
	virtual void Set(char const* monthAbbr, uint32_t day, uint32_t year) { cRZDate::Set(monthAbbr, day, year); }

	virtual void Copy(cIGZDate const& src);
	virtual void Clone(cIGZDate** out);

	virtual uint32_t Between(cIGZDate const& lower, cIGZDate const& upper) const;
	virtual int CompareTo(cIGZDate const& other) const;

	virtual uint32_t WeekDay() const { return cRZDate::WeekDay(); }
	virtual uint32_t Year() const { return cRZDate::Year(); }
	virtual uint32_t DayOfYear() const { return cRZDate::DayOfYear(); }
	virtual uint32_t DayOfMonth() const { return cRZDate::DayOfMonth(); }
	virtual uint32_t FirstDayOfMonth() const;
	virtual uint32_t FirstDayOfMonth(uint32_t month) const { return cRZDate::FirstDayOfMonth(month); }
	virtual uint32_t Hash() const;
	
	virtual bool IsValid() const;
	virtual uint32_t Leap() const;

	virtual void MaxDate(cIGZDate const& other, cIGZDate& result) const;
	virtual void MinDate(cIGZDate const& other, cIGZDate& result) const;

	virtual uint32_t Month() const { return cRZDate::Month(); }

	virtual void Previous(uint32_t dayOfWeek, cIGZDate& result) const;

	virtual uint32_t YearLastTwoDigits() const;
	virtual uint32_t DayNumber() const;

public:
	virtual bool operator<  (cIGZDate const& other) const;
	virtual bool operator<= (cIGZDate const& other) const;
	virtual bool operator>  (cIGZDate const& other) const;
	virtual bool operator>= (cIGZDate const& other) const;
	virtual bool operator== (cIGZDate const& other) const;
	virtual bool operator!= (cIGZDate const& other) const;

	virtual int32_t operator- (cIGZDate const& other) const;
	virtual int32_t operator- (int32_t days) const;

	virtual int32_t operator+ (cIGZDate const& other) const;
	virtual int32_t operator+ (int32_t days) const;

	virtual cIGZDate& operator++ (); // pre-increment
	virtual cIGZDate& operator-- (); // pre-increment

	virtual cIGZDate& operator+= (int32_t days);
	virtual cIGZDate& operator-= (int32_t days);

public:
	virtual void DateString(cIGZString& out, uint32_t format) const;
	virtual uint32_t DayWithinMonth(uint32_t month, uint32_t day, uint32_t year);
	virtual uint32_t DaysInYear(uint32_t year);
	virtual uint32_t Jday(uint32_t month, uint32_t day, uint32_t year);
	virtual uint32_t LeapYear(uint32_t year);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

protected:
	uint32_t mRefCount;
};