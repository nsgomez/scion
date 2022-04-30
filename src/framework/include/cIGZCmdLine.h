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

static const GZIID GZIID_cIGZCmdLine = 0xA31E539C;

class cIGZString;
class cRZString;

class cIGZCmdLine : public cIGZUnknown
{
public:
	virtual int argc() const = 0;
	virtual char const* argv(int index) const = 0;
	virtual bool GetCommandLineText(cIGZString& out) const = 0;

	virtual int Find(cIGZString const& searchTerm, bool caseSensitive = true) const = 0;
	virtual int FindSubString(cIGZString const& searchTerm, bool caseSensitive = true) const = 0;

	virtual bool IsSwitchPresent(char option, cIGZString& valueOut, bool unknown) const = 0; // TODO
	virtual int GetIndexOfSwitch(char option, int unknown) const = 0; // TODO

	virtual bool IsSwitchPresent(cIGZString const& searchTerm) const = 0;
	virtual bool IsSwitchPresent(cIGZString const& searchTerm, cIGZString& valueOut, bool unknown) const = 0; // TODO
	virtual int GetIndexOfSwitch(cIGZString const& searchTerm, int unknown) const = 0; // TODO

	virtual bool InsertArgument(cIGZString const& arg, int unknown) = 0; // TODO
	virtual bool EraseArgument(int index) = 0;

	virtual cRZString const& operator[] (int index) const = 0;
};