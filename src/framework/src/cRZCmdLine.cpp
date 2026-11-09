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

#include "cRZCmdLine.h"

cRZCmdLine::cRZCmdLine() :
	mRefCount(0),
	mArguments(),
	mFullText(),
	mEmptyStr()
{
}

cRZCmdLine::cRZCmdLine(cIGZCmdLine const& cmdLine) :
	mRefCount(0),
	mArguments(),
	mFullText(),
	mEmptyStr()
{
	cmdLine.GetCommandLineText(mFullText);
	for (int i = 0; i < cmdLine.argc(); i++)
	{
		mArguments.push_back(cRZString(cmdLine.argv(i)));
	}
}

cRZCmdLine::cRZCmdLine(char const* cmdLine) :
	mRefCount(0),
	mArguments(0),
	mFullText(cmdLine),
	mEmptyStr()
{
	ConvertStringArrayToString(mFullText, mArguments);
}

bool cRZCmdLine::QueryInterface(GZIID iid, void** outPtr)
{
	switch (iid)
	{
	case GZIID_cIGZUnknown:
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;

	case GZIID_cIGZCmdLine:
		*outPtr = static_cast<cIGZCmdLine*>(this);
		AddRef();
		return true;
	}

	return false;
}

uint32_t cRZCmdLine::AddRef(void)
{
	return ++mRefCount;
}

uint32_t cRZCmdLine::Release(void)
{
	if (mRefCount > 1)
	{
		return --mRefCount;
	}
	else
	{
		delete this;
		return 0;
	}
}

int cRZCmdLine::argc() const
{
	return mArguments.size();
}

char const* cRZCmdLine::argv(int index) const
{
	if (index < (int)mArguments.size())
	{
		return mArguments[index].ToChar();
	}

	return mEmptyStr.ToChar();
}

bool cRZCmdLine::GetCommandLineText(cIGZString& out) const
{
	out.Copy(mFullText);
	return true;
}

int cRZCmdLine::Find(cIGZString const& searchTerm, bool caseSensitive) const
{
	size_t count = mArguments.size();

	cRZString needle(searchTerm);
	cRZString tmp;

	if (!caseSensitive)
	{
		needle.MakeLower();
	}

	for (int i = 0; i < count; i++)
	{
		if (caseSensitive)
		{
			if (needle == mArguments[i])
			{
				return i;
			}
		}
		else
		{
			tmp = mArguments[i];
			tmp.MakeLower();

			if (needle == tmp)
			{
				return i;
			}
		}
	}

	return -1;
}

int cRZCmdLine::FindSubString(cIGZString const& searchTerm, bool caseSensitive) const
{
	size_t count = mArguments.size();

	cRZString needle(searchTerm);
	cRZString tmp;

	if (!caseSensitive)
	{
		needle.MakeLower();
	}

	for (int i = 0; i < count; i++)
	{
		int findResult;
		size_t length;

		if (caseSensitive)
		{
			findResult = (int)mArguments[i].find(needle.ToChar(), 0, needle.Strlen());
			length = mArguments[i].Strlen();
		}
		else
		{
			tmp = mArguments[i];
			tmp.MakeLower();

			findResult = (int)tmp.find(needle.ToChar(), 0, needle.Strlen());
			length = tmp.Strlen();
		}

		if (findResult < length)
		{
			return i;
		}
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
	int i = startIndex;
	if (i < 0)
	{
		i = 0;
	}

	if (i < (int)mArguments.size())
	{
		cRZString delims("/-\\");
		char upper, lower;

		if (option >= 'A' && option <= 'Z')
		{
			upper = option;
			lower = option + 0x20;
		}
		else if (option >= 'a' && option <= 'z')
		{
			lower = option;
			upper = option - 0x20;
		}
		else
		{
			upper = option;
			lower = option;
		}

		size_t count = mArguments.size();
		while (i < count)
		{
			cRZString const& arg = mArguments[i];
			if (arg.length() == 2)
			{
				char switchChar = arg[1];
				if (delims.find(arg[0], 0) < delims.Strlen() && (switchChar == lower || switchChar == upper))
				{
					return i;
				}
			}

			i++;
		}
	}

	return -1;
}

bool cRZCmdLine::IsSwitchPresent(cIGZString const& searchTerm) const
{
	cRZString value;
	return GetIndexOfSwitch(searchTerm, 0) >= 0 || IsSwitchPresent(searchTerm, value, true);
}

bool cRZCmdLine::IsSwitchPresent(cIGZString const& searchTerm, cIGZString& valueOut, bool trimValueQuotes) const
{
	if (searchTerm.Strlen() != 0)
	{
		cRZString needle(searchTerm);
		needle.MakeLower();

		cRZString delims("/-\\");
		for (tArgList::const_iterator it = mArguments.begin(); it != mArguments.end(); ++it)
		{
			cRZString const& arg = *it;

			// Check that the argument has a valid delimiter
			size_t delimIndex = 0;
			while (delimIndex < delims.length() && arg[0] != delims[delimIndex])
			{
				delimIndex++;
			}

			if (delimIndex == delims.Strlen())
			{
				continue;
			}

			// Check that the argument has the proper length to include the delimiter,
			// needle string, ':' separator, and at least one character in the value.
			size_t argLength = arg.Strlen();
			if (argLength < needle.Strlen() + 3)
			{
				continue;
			}

			// Check that the ':' separator is in the right place
			if (arg[needle.Strlen() + 1] != ':')
			{
				continue;
			}

			// Check that the name of the argument is what we're looking for
			cRZString tmp(arg);
			tmp.MakeLower();

			size_t i = 0;
			while (i < needle.Strlen() && needle[i] == tmp[i + 1])
			{
				i++;
			}

			if (i != needle.Strlen())
			{
				continue;
			}

			// Get the value from the arg string
			cRZString value(arg, needle.Strlen() + 2);
			if (trimValueQuotes)
			{
				char first = value[0];
				if (first == '"' || first == '\'')
				{
					char last = value[value.Strlen() - 1];
					if (last == '"' || last == '\'')
					{
						value.Erase(0, 1);
						value.Erase(value.Strlen() - 1, 1);
					}
				}
			}

			valueOut.Copy(value);
			return true;
		}
	}

	return false;
}

int cRZCmdLine::GetIndexOfSwitch(cIGZString const& searchTerm, int startIndex) const
{
	if (startIndex < 0)
	{
		startIndex = 0;
	}

	if (startIndex < (int)mArguments.size())
	{
		cRZString delims("/-\\");
		size_t count = mArguments.size();

		for (int i = 0; i < count; i++)
		{
			cRZString const& arg = mArguments[i];
			if (arg.Strlen() < 2)
			{
				continue;
			}

			if (delims.find(arg[0], 0) >= delims.Strlen())
			{
				continue;
			}

			if (searchTerm.Strlen() != arg.Strlen() - 1)
			{
				continue;
			}

			cRZString needle(searchTerm);
			needle.MakeLower();

			cRZString option(arg);
			option.Erase(0, 1);
			option.MakeLower();

			if (option.find(needle.ToChar(), 0, needle.Strlen()) == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

bool cRZCmdLine::InsertArgument(cIGZString const& arg, int index)
{
	int size = (int)mArguments.size();
	int insertAt;

	if (size == 0)
	{
		insertAt = 0;
	}
	else
	{
		insertAt = index;
		if (insertAt < 0)
		{
			insertAt = 0;
		}
		else if (insertAt >= size)
		{
			insertAt = size;
		}
	}

	mArguments.insert(mArguments.begin() + insertAt, cRZString(arg));
	ConvertStringArrayToString(mArguments, mFullText);
	return true;
}

bool cRZCmdLine::EraseArgument(int index)
{
	if (index >= (int)mArguments.size())
	{
		index = (int)mArguments.size() - 1;
	}

	mArguments.erase(mArguments.begin() + index);
	ConvertStringArrayToString(mArguments, mFullText);
	return true;
}

cRZString const& cRZCmdLine::operator[] (int index) const
{
	if (index < (int)mArguments.size())
	{
		return mArguments[index];
	}

	return mEmptyStr;
}

void cRZCmdLine::ConvertStringArrayToString(cRZString const& str, std::vector<cRZString>& array)
{
	cRZString token;
	cRZString strCopy(str);
	array.clear();

	// Trim whitespace off the end of the command line string
	while (strCopy.Strlen() != 0)
	{
		int last = (int)strCopy.Strlen() - 1;
		if (strCopy[last] != '\n' && strCopy[last] != '\t' && strCopy[last] != ' ')
		{
			break;
		}

		strCopy.Erase(strCopy.Strlen() - 1, 1);
	}

	// Trim whitespace off the start of the command line string
	while (strCopy.Strlen() != 0 && (strCopy[0] == '\n' || strCopy[0] == '\t' || strCopy[0] == ' '))
	{
		strCopy.Erase(0, 1);
	}

	// Start slicing the arguments
	int length = (int)strCopy.Strlen();
	if (length != 0)
	{
		int quoteCount = 0;
		int tokenStart = 0;

		for (int i = 0; i < length; i++)
		{
			char c = strCopy[i];

			// Found an argument separator - add the token to the argument array...
			if (c == '\n' || c == '\t' || c == ' ')
			{
				// ... unless we're in the middle of a quote string
				if (quoteCount & 1)
				{
					continue;
				}

				quoteCount = 0;

				char const* tokenData = strCopy.ToChar() + tokenStart;
				token.assign(tokenData, tokenData + (i - tokenStart));

				// Trim quotes from the beginning and end of the token
				if (token[0] == '"' && token[token.Strlen() - 1] == '"')
				{
					token.Erase(token.Strlen() - 1, 1);
					if (token.Strlen() != 0)
					{
						token.Erase(0, 1);
					}
				}

				array.push_back(token);

				// Skip extra whitespace between this argument and the next one
				while (strCopy[i + 1] == '\n' || strCopy[i + 1] == '\t' || strCopy[i + 1] == ' ')
				{
					i++;
				}

				tokenStart = i + 1;
			}
			else if (c == '"')
			{
				quoteCount++;
			}
		}

		// After leaving the loop above, there'll be one last token that we need to add
		char const* tokenData = strCopy.ToChar() + tokenStart;
		token.assign(tokenData, tokenData + (i - tokenStart));

		if (token[0] == '"' && token[token.Strlen() - 1] == '"')
		{
			token.Erase(token.Strlen() - 1, 1);
			if (token.Strlen() != 0)
			{
				token.Erase(0, 1);
			}
		}

		array.push_back(token);
	}
}

void cRZCmdLine::ConvertStringArrayToString(std::vector<cRZString> const& array, cRZString& str)
{
	str.Erase();
	for (size_t i = 0, count = array.size(); i < count; i++)
	{
		size_t length = array[i].length();
		bool quote = false;

		for (size_t j = 0; j < length; j++)
		{
			if (array[i][j] == '\n' || array[i][j] == '\t' || array[i][j] == ' ')
			{
				quote = true;
				str += '"';
				break;
			}
		}

		str += array[i];
		if (quote)
		{
			str += '"';
		}

		if (i < count - 1)
		{
			str += ' ';
		}
	}
}