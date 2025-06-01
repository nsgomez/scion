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

#include "cRZHashStringCRCInitializer.h"
#include "nRZHash.h"

cRZHashStringCRCInitializer::cRZHashStringCRCInitializer(void)
{
	if (!nRZHash::gbInitialized)
	{
		nRZHash::gbInitialized = true;

		for (int i = 0; i < 256; i++)
		{
			uint32_t crc = i << 16;
			for (int j = 0; j < 8; j++)
			{
				crc <<= 1;
				if (crc & 0x01000000)
				{
					crc ^= 0x01864CFB;
				}
			}

			nRZHash::gCRC24Table[i] = crc;
		}

		for (int i = 0; i < 256; i++)
		{
			uint32_t crc = i << 24;
			for (int j = 0; j < 8; j++)
			{
				crc = (crc & 0x80000000)
					? (crc << 1) ^ 0x04C11DB7
					: (crc << 1);
			}

			nRZHash::gCRC32Table[i] = crc;
		}
	}
}