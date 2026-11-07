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

#include <ctype.h>
#include "cRZHashStringCRCInitializer.h"
#include "nRZHash.h"

namespace nRZHash
{
	uint32_t CRC24(char const* data, bool caseInsensitive)
	{
		if (!gbInitialized)
		{
			cRZHashStringCRCInitializer init;
		}

		uint32_t crc = 0x00B704CE;
		while (*data)
		{
			char c = *data;
			int intChar = c;

			if (caseInsensitive)
			{
				intChar = tolower(intChar);
			}

			crc = gCRC24Table[((crc >> 16) ^ intChar) & 0xFF] ^ (crc << 8);
			++data;
		}

		return crc & 0xFFFFFF;
	}

	uint32_t CRC32(uint8_t const* data, uint32_t length)
	{
		if (!gbInitialized)
		{
			cRZHashStringCRCInitializer init;
		}

		uint32_t crc = -1;
		while (length--)
		{
			crc = gCRC32Table[((crc >> 24) ^ *(data++)) & 0xFF] ^ (crc << 8);
		}

		return crc;
	}

	uint32_t gCRC32Table[256];
	uint32_t gCRC24Table[256];
	bool gbInitialized = false;
};