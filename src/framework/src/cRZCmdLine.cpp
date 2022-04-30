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

#include "cRZCmdLine.h"

cRZCmdLine::cRZCmdLine()
{
}

cRZCmdLine::cRZCmdLine(cIGZCmdLine const& cmdLine)
{
	// TODO
}

cRZCmdLine::cRZCmdLine(char const* cmdLine)
{
	// TODO
}

cRZCmdLine::~cRZCmdLine()
{
}

bool cRZCmdLine::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZCmdLine)
	{
		*outPtr = static_cast<cIGZCmdLine*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZCmdLine::AddRef(void)
{
	return ++refCount;
}

uint32_t cRZCmdLine::Release(void)
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

int cRZCmdLine::argc() const
{
	return arguments.size();
}

char const* cRZCmdLine::argv(int index) const
{
	if (index >= arguments.size())
	{
		return unknownstr.ToChar();
	}
	else
	{
		return arguments[index].ToChar();
	}
}

bool cRZCmdLine::GetCommandLineText(cIGZString& out) const
{
	out.Copy(fullText);
	return true;
}

int cRZCmdLine::Find(cIGZString const& searchTerm, bool caseSensitive) const
{
	tArgList::const_iterator it = arguments.begin();
	tArgList::const_iterator itEnd = arguments.end();

	cRZString needle(searchTerm);
	cRZString tmpLower;

	if (!caseSensitive)
	{
		needle.MakeLower();
	}

	if (it != itEnd)
	{
		int index = 0;
		do
		{
			cRZString const* haystack = it;
			if (!caseSensitive)
			{
				tmpLower = *haystack;
				tmpLower.MakeLower();
				haystack = &tmpLower;
			}

			if (needle.CompareTo(*haystack) == 0)
			{
				return index;
			}

			++index;
			++it;
		}
		while (index < arguments.size());
	}

	return -1;
}

int cRZCmdLine::FindSubString(cIGZString const& searchTerm, bool caseSensitive) const
{
	tArgList::const_iterator it = arguments.begin();
	tArgList::const_iterator itEnd = arguments.end();

	cRZString needle(searchTerm);
	cRZString tmpLower;

	if (!caseSensitive)
	{
		needle.MakeLower();
	}

	if (it != itEnd)
	{
		int index = 0;
		do
		{
			cRZString const* haystack;
			size_t findResult;

			if (!caseSensitive)
			{
				tmpLower = *it;
				tmpLower.MakeLower();
				findResult = tmpLower.Find(needle);
				haystack = &tmpLower;
			}
			else
			{
				findResult = it->Find(needle);
				haystack = it;
			}

			if (findResult < haystack->Strlen())
			{
				return index;
			}

			++index;
			++it;
		}
		while (index < arguments.size());
	}

	return -1;
}

bool cRZCmdLine::IsSwitchPresent(char option, cIGZString& valueOut, bool unknown) const
{
	// TODO
	return false;
}

int cRZCmdLine::GetIndexOfSwitch(char option, int unknown) const
{
	// TODO
	return -1;
}

bool cRZCmdLine::IsSwitchPresent(cIGZString const& searchTerm) const
{
	// TODO
	return false;
}

bool cRZCmdLine::IsSwitchPresent(cIGZString const& searchTerm, cIGZString& valueOut, bool unknown) const
{
	// TODO
	return false;
}

int cRZCmdLine::GetIndexOfSwitch(cIGZString const& searchTerm, int unknown) const
{
	// TODO
	return -1;
}

bool cRZCmdLine::InsertArgument(cIGZString const& arg, int unknown)
{
	// TODO
	return false;
}

bool cRZCmdLine::EraseArgument(int index)
{
	// TODO
	return false;
}

cRZString const& cRZCmdLine::operator[] (int index) const
{
	if (index >= arguments.size())
	{
		return unknownstr;
	}
	else
	{
		return arguments[index];
	}
}

void cRZCmdLine::ConvertStringArrayToString(std::vector<cRZString> const& array, cRZString& str)
{
	str.Erase();

	if (array.size() == 0)
	{
		return;
	}

	while (true)
	{
		// TODO
	}
}