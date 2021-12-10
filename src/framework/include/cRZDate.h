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
#include <stdint.h>

class cGZDate;
class cIGZDate;
class cRZString;

class cRZDate
{
public:
	cRZDate();
	cRZDate(cIGZDate const& date);
	cRZDate(cGZDate const& date);
	cRZDate(cRZDate const& date);
	cRZDate(uint32_t dayNumber);
	cRZDate(uint32_t day, uint32_t year);
	cRZDate(uint32_t month, uint32_t day, uint32_t year);

public:
	uint32_t DayOfMonth() const;
	uint32_t DayOfYear() const;
	uint32_t FirstDayOfMonth(uint32_t month) const;
	void Mdy(uint32_t& month, uint32_t& day, uint32_t& year) const;
	uint32_t Month() const;
	uint32_t Year() const;

	void Set(char const* monthAbbr, uint32_t day, uint32_t year);
	void Set(uint32_t month, uint32_t day, uint32_t year);
	void Set(uint32_t newDateCode);

public:
	static bool DayWithinMonth(uint32_t month, uint32_t day, uint32_t year);
	static uint32_t Jday(uint32_t month, uint32_t day, uint32_t year);

public:
	virtual void NameOfDayString(cRZString& out, uint32_t format) const;
	virtual void NameOfMonthString(cRZString& out, uint32_t format) const;
	virtual void DateString(cRZString& out, uint32_t format) const;

protected:
	uint32_t dateCode;
	friend class cGZDate;
};