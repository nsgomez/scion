/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>

struct cGZPersistResourceKey
{
public:
	cGZPersistResourceKey() :
		type(0),
		group(0),
		instance(0)
	{
	}

	cGZPersistResourceKey(uint32_t type, uint32_t group, uint32_t instance) :
		type(type),
		group(group),
		instance(instance)
	{
	}

	cGZPersistResourceKey(const cGZPersistResourceKey& other)
	{
		type = other.type;
		group = other.group;
		instance = other.instance;
	}

	cGZPersistResourceKey& operator=(const cGZPersistResourceKey& other)
	{
		type = other.type;
		group = other.group;
		instance = other.instance;

		return *this;
	}

	bool operator== (cGZPersistResourceKey const& other)
	{
		return type == other.type && group == other.group && instance == other.instance;
	}

	bool operator< (cGZPersistResourceKey const& other)
	{
		return type < other.type || group < other.group || instance < other.instance;
	}

	bool operator> (cGZPersistResourceKey const& other)
	{
		return !(*this < other);
	}

	friend bool operator==(cGZPersistResourceKey const& left, cGZPersistResourceKey const& right)
	{
		return left.type == right.type && left.group == right.group && left.instance == right.instance;
	}

	friend bool operator< (cGZPersistResourceKey const& left, cGZPersistResourceKey const& right)
	{
		return left.type < right.type || left.group < right.group || left.instance < right.instance;
	}

	friend bool operator> (cGZPersistResourceKey const& left, cGZPersistResourceKey const& right)
	{
		return !(left < right);
	}

public:
	uint32_t type;
	uint32_t group;
	uint32_t instance;
};