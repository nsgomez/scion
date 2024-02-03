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
	: refCount(0), arguments(), fullText(), emptyStr()
{
	cmdLine.GetCommandLineText(fullText);
	for (int i = 0; i < cmdLine.argc(); i++)
	{
		arguments.push_back(cRZString(cmdLine.argv(i)));
	}
}

cRZCmdLine::cRZCmdLine(char const* cmdLine)
	: refCount(0), arguments(), fullText(cmdLine), emptyStr()
{
	ConvertStringArrayToString(fullText, arguments);
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
		return emptyStr.ToChar();
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

int cRZCmdLine::GetIndexOfSwitch(char option, int startIndex) const
{
	size_t index = 0;
	if (startIndex > -1)
	{
		index = startIndex;
	}

	if (!arguments.empty())
	{
		std::string delimiters("/-\\");
		char upperFlag, lowerFlag;

		if (option >= 'A' && option <= 'Z')
		{
			upperFlag = option;
			lowerFlag = option + ' ';
		}
		else
		{
			upperFlag = option;
			if (option >= 'a' && option <= 'z')
			{
				upperFlag = option - ' ';
			}

			lowerFlag = option;
		}

		for (; startIndex < arguments.size(); startIndex++)
		{
			cRZString const& arg = arguments[startIndex];
			if (arg.length() == 2)
			{
				char argFlag = arg[1];
				size_t delimiterOffset = delimiters.find(arg[0]);

				if (delimiterOffset < delimiters.length() && (argFlag == upperFlag || argFlag == lowerFlag))
				{
					return index;
				}
			}
		}
	}

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

bool cRZCmdLine::InsertArgument(cIGZString const& arg, int index)
{
	int insertAt = 0;
	if (!arguments.empty() && index > -1 && arguments.size() - index != 0 && index <= arguments.size())
	{
		insertAt = index;
	}

	arguments.insert(arguments.begin() + insertAt, cRZString(arg.Data(), arg.Strlen()));
	ConvertStringArrayToString(arguments, fullText);
	return true;
}

bool cRZCmdLine::EraseArgument(int index)
{
	arguments.erase(arguments.begin() + index);
	ConvertStringArrayToString(arguments, fullText);
	return true;
}

cRZString const& cRZCmdLine::operator[] (int index) const
{
	if (index >= arguments.size())
	{
		return emptyStr;
	}
	else
	{
		return arguments[index];
	}
}

void cRZCmdLine::ConvertStringArrayToString(cRZString const& str, std::vector<cRZString>& array)
{
	cRZString tempArg;
	cRZString strCopy(str);
	array.clear();

	size_t length = strCopy.length();
	while (length != 0)
	{
		if (strCopy[length - 1] >= ' ' && strCopy[length - 1] <= '~')
		{
			break;
		}

		strCopy.erase(length - 1, 1);
		length = strCopy.length();
	}

	while (length != 0)
	{
		if (strCopy[length - 1] >= ' ' && strCopy[length - 1] <= '~')
		{
			break;
		}

		strCopy.erase(0, 1);
		length = strCopy.length();
	}

	if (length == 0)
	{
		return;
	}

	// TODO
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