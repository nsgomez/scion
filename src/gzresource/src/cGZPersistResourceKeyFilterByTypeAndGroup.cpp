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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGZPersistResourceKey.h"
#include "cGZPersistResourceKeyFilterByTypeAndGroup.h"

cGZPersistResourceKeyFilterByTypeAndGroup::cGZPersistResourceKeyFilterByTypeAndGroup(uint32_t type, uint32_t group) :
	cGZPersistResourceKeyFilter(),
	type(type),
	group(group)
{
}

bool cGZPersistResourceKeyFilterByTypeAndGroup::IsKeyIncluded(cGZPersistResourceKey const& key)
{
	return key.type == type && key.group == group;
}