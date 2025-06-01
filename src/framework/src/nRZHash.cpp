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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <ctype.h>
#include "cRZHashStringCRCInitializer.h"
#include "nRZHash.h"

namespace nRZHash
{
	static uint32_t CRC24(char const* data, bool caseInsensitive)
	{
		if (!gbInitialized)
		{
			cRZHashStringCRCInitializer init;
		}
		
		uint32_t crc = 0x00B704CE;
		char c;

		while ((c = *(data++)) != 0)
		{
			if (caseInsensitive)
			{
				c = tolower(c);
			}

			crc = gCRC24Table[((crc >> 16) ^ c) & 0xFF] ^ (crc << 8);
		}

		return crc;
	}

	static uint32_t CRC32(char const* data, uint32_t length)
	{
		if (!gbInitialized)
		{
			cRZHashStringCRCInitializer init;
		}

		uint32_t crc = -1;
		while (length--)
		{
			crc = gCRC32Table[(crc ^ *(data++)) & 0xFF] ^ (crc >> 8);
		}

		return crc;
	}

	bool gbInitialized = false;
	uint32_t gCRC24Table[256];
	uint32_t gCRC32Table[256];
};