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

#pragma once
#include "cIGZUnknown.h"

static const GZIID GZIID_cIGZDate = 0x43378255;

class cIGZString;
class cRZTime;

class cIGZDate : public cIGZUnknown
{
public:
	virtual void Set(uint32_t day, uint32_t year) = 0;
	virtual void Set(uint32_t month, uint32_t day, uint32_t year) = 0;
	virtual void Set(cRZTime const& time) = 0;
	virtual void Set(uint32_t dateCode) = 0;
	virtual void Set(char const* monthAbbr, uint32_t day, uint32_t year) = 0;

	virtual void Copy(cIGZDate const& src) = 0;
	virtual void Clone(cIGZDate** out) = 0;

	virtual bool Between(cIGZDate const& lower, cIGZDate const& upper) const = 0;
	virtual int CompareTo(cIGZDate const& other) const = 0;

	virtual uint32_t WeekDay() const = 0;
	virtual uint32_t Year() const = 0;
	virtual uint32_t DayOfYear() const = 0;
	virtual uint32_t DayOfMonth() const = 0;
	virtual uint32_t FirstDayOfMonth() const = 0;
	virtual uint32_t FirstDayOfMonth(uint32_t month) const = 0;
	virtual uint32_t Hash() const = 0;
	
	virtual bool IsValid() const = 0;
	virtual bool Leap() const = 0;

	virtual void MaxDate(cIGZDate const& other, cIGZDate& result) const = 0;
	virtual void MinDate(cIGZDate const& other, cIGZDate& result) const = 0;

	virtual uint32_t Month() const = 0;

	virtual void Previous(uint32_t dayOfWeek, cIGZDate& result) const = 0;

	virtual uint32_t YearLastTwoDigits() const = 0;
	virtual uint32_t DayNumber() const = 0;

public:
	virtual bool operator<  (cIGZDate const& other) const = 0;
	virtual bool operator<= (cIGZDate const& other) const = 0;
	virtual bool operator>  (cIGZDate const& other) const = 0;
	virtual bool operator>= (cIGZDate const& other) const = 0;
	virtual bool operator== (cIGZDate const& other) const = 0;
	virtual bool operator!= (cIGZDate const& other) const = 0;

	virtual int32_t operator- (cIGZDate const& other) const = 0;
	virtual int32_t operator+ (cIGZDate const& other) const = 0;

	virtual int32_t operator- (int32_t days) const = 0;
	virtual int32_t operator+ (int32_t days) const = 0;

	virtual cIGZDate& operator++ () = 0;
	virtual cIGZDate& operator-- () = 0;

	virtual cIGZDate& operator+= (int32_t days) = 0;
	virtual cIGZDate& operator-= (int32_t days) = 0;

public:
	virtual void DateString(cIGZString& out, uint32_t format) const = 0;
	virtual bool DayWithinMonth(uint32_t month, uint32_t day, uint32_t year) = 0;
	virtual uint32_t DaysInYear(uint32_t year) = 0;
	virtual uint32_t Jday(uint32_t month, uint32_t day, uint32_t year) = 0;
	virtual bool LeapYear(uint32_t year) = 0;
};