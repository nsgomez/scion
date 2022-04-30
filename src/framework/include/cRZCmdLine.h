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
#include <vector>
#include "cIGZCmdLine.h"
#include "cRZString.h"

class cRZCmdLine : public cIGZCmdLine
{
private:
	typedef std::vector<cRZString> tArgList;

public:
	cRZCmdLine();
	cRZCmdLine(cIGZCmdLine const& cmdLine);
	cRZCmdLine(char const* cmdLine);

public:
	virtual bool QueryInterface(GZIID iid, void** outPtr);
	virtual uint32_t AddRef(void);
	virtual uint32_t Release(void);

public:
	virtual int argc() const;
	virtual char const* argv(int index) const;
	virtual bool GetCommandLineText(cIGZString& out) const;

	virtual int Find(cIGZString const& searchTerm, bool caseSensitive = true) const;
	virtual int FindSubString(cIGZString const& searchTerm, bool caseSensitive = true) const;

	virtual bool IsSwitchPresent(char option, cIGZString& valueOut, bool unknown) const; // TODO
	virtual int GetIndexOfSwitch(char option, int unknown) const; // TODO

	virtual bool IsSwitchPresent(cIGZString const& searchTerm) const;
	virtual bool IsSwitchPresent(cIGZString const& searchTerm, cIGZString& valueOut, bool unknown) const; // TODO
	virtual int GetIndexOfSwitch(cIGZString const& searchTerm, int unknown) const; // TODO

	virtual bool InsertArgument(cIGZString const& arg, int unknown); // TODO
	virtual bool EraseArgument(int index);

	virtual cRZString const& operator[] (int index) const;

protected:
	void ConvertStringArrayToString(std::vector<cRZString> const& array, cRZString& str);

	uint32_t refCount;
	tArgList arguments;
	cRZString fullText;
	cRZString unknownstr; // TODO
};