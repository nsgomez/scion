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

#include <algorithm>
#include <cstdlib>
#include "cRZFastCompression3.h"
#include "cRZString.h"

cRZFastCompression3::cRZFastCompression3(void) :
	refCount(0),
	quickCompressionEnabled(false)
{
}

bool cRZFastCompression3::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid == GZIID_cIGZUnknown)
	{
		*outPtr = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}
	else if (iid == GZIID_cIGZCompression)
	{
		*outPtr = static_cast<cIGZCompression*>(this);
		AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t cRZFastCompression3::AddRef(void)
{
	return ++refCount;
}

uint32_t cRZFastCompression3::Release(void)
{
	if (refCount > 1)
	{
		return --refCount;
	}

	delete this;
	return 0;
}

uint32_t cRZFastCompression3::GetMaxLengthRequiredForCompressedData(uint32_t size)
{
	return ((size * 15) / 10) + 1024;
}

uint32_t cRZFastCompression3::GetLengthOfCompressedData(uint8_t const* data)
{
	return *reinterpret_cast<uint32_t const*>(data);
}

static uint32_t decodeintref(uint8_t const* data, int numBytes)
{
	uint32_t result = 0;
	for (int i = 0; i < numBytes; i++)
	{
		result = (result << 8) + *(data++);
	}

	return result;
}

static uint32_t sizeref(uint8_t const* data)
{
	uint32_t signature = decodeintref(data, 2);
	if (signature == 0x10FB)
	{
		return decodeintref(&data[2], 3);
	}
	else
	{
		return decodeintref(&data[5], 3);
	}
}

uint32_t cRZFastCompression3::GetLengthOfDecompressedData(uint8_t const* data)
{
	return sizeref(&data[4]);
}

void cRZFastCompression3::GetCompressionVersionString(cIGZString& str)
{
	str.Copy(cRZString("0.90"));
}

void cRZFastCompression3::EnableQuickCompression(bool flag)
{
	quickCompressionEnabled = flag;
}

static bool isref(uint8_t const* data)
{
	uint32_t signature = (data[0] << 8) | data[1];
	return signature == 0x10FB || signature == 0x11FB || signature == 0x90FB || signature == 0x91FB;
}

bool cRZFastCompression3::ValidateCompressedData(uint8_t const* data, uint32_t size)
{
	return size > 6
		&& isref(data)
		&& GetLengthOfCompressedData(data) <= kMaxRefPackDataLength
		&& GetLengthOfDecompressedData(data) <= kMaxRefPackDataLength;
}

static void encodeintref(uint8_t* data, int value, int numBytes)
{
	value <<= (((4 - numBytes) * 8) & 31);
	for (int i = 0; i < numBytes; i++)
	{
		*(data++) = (uint8_t)(value >> 24);
		value <<= 8;
	}
}

inline uint32_t refhash(uint8_t* data)
{
	return ((uint32_t)(data[1]) << 4)
		 ^ ((uint32_t)((data[0] << 8) | data[2]))
		 * 4;
}

static uint32_t matchlen(uint8_t const* src, uint8_t const* dest, uint32_t maxLength)
{
	uint32_t current = 0;
	while (current < maxLength && *(src++) == *(dest++))
	{
		++current;
	}

	return current;
}

static uint32_t refpack(uint8_t* in, uint32_t inSize, uint8_t* out, uint32_t /* unused */, int fast)
{
	// This code is mostly derived from Frank Barchard's canonical implementation of RefPack for Origin Software, since
	// that seems to be the closest ancestor to the game code.

	const uint32_t kRefPackHashTableSize = 65536;
	const uint32_t kRefPackWindowLength = (1 << 17);
	const uint32_t kRefPackWindowMask = kRefPackWindowLength - 1;

	uint32_t* hashtbl = (uint32_t*)malloc(kRefPackHashTableSize * sizeof(uint32_t));
	if (hashtbl == NULL)
	{
		return 0;
	}

	uint32_t* link = (uint32_t*)malloc(kRefPackWindowLength * sizeof(uint32_t));
	if (link == NULL)
	{
		return 0;
	}

	uint32_t* tmp = hashtbl;
	for (int i = 0; i < kRefPackHashTableSize / 16; i++)
	{
		tmp[0]  = tmp[1]  = tmp[2]  = tmp[3]  =
		tmp[4]  = tmp[5]  = tmp[6]  = tmp[7]  =
		tmp[8]  = tmp[9]  = tmp[10] = tmp[11] =
		tmp[12] = tmp[13] = tmp[14] = tmp[15] = -1;

		tmp += 16;
	}

	uint32_t run = 0;
	uint32_t tlen;
	uint32_t toffset;

	uint8_t* cptr = in;
	uint8_t* rptr = in;
	uint8_t* out_orig = out;

	while (inSize > 0)
	{
		uint32_t boffset = 0;
		uint32_t blen = 2;
		uint32_t bcost = 2;

		uint32_t mlen = std::min<uint32_t>(inSize, 1028);
		uint8_t* tptr = cptr - 1;

		int32_t hash = refhash(cptr);
		int32_t hoffset = hashtbl[hash];
		int32_t index = cptr - in;
		int32_t minhoffset = std::max<int32_t>(index - kRefPackWindowMask, 0);

		if (hoffset >= minhoffset)
		{
			do
			{
				tptr = reinterpret_cast<uint8_t*>(in) + hoffset;
				if (cptr[blen] == tptr[blen])
				{
					uint32_t tlen = matchlen(cptr, tptr, mlen);
					if (tlen > blen)
					{
						uint32_t toffset = (cptr - 1) - tptr;
						uint32_t tcost;

						if (toffset < 1024 && tlen <= 10)
						{
							// two byte long form
							tcost = 2;
						}
						else if (toffset < 16384 && tlen <= 67)
						{
							// three byte long form
							tcost = 3;
						}
						else
						{
							// four byte very long form
							tcost = 4;
						}

						if (tlen - tcost + 4 > blen - bcost + 4)
						{
							blen = tlen;
							bcost = tcost;
							boffset = toffset;

							if (blen >= 1028)
							{
								break;
							}
						}
					}
				}

				tlen = blen;
				toffset = boffset;
				hoffset = link[hoffset & kRefPackWindowMask];
				blen = tlen;
			}
			while (hoffset >= minhoffset);
		}

		if (bcost >= blen)
		{
			hoffset = index;

			++run;
			--inSize;

			link[hoffset & kRefPackWindowMask] = hashtbl[hash];
			hashtbl[hash] = hoffset;
			continue;
		}

		while (run > 3)
		{
			// literal block of data
			tlen = std::min<uint32_t>(112, run & (~3));
			run -= tlen;
			*(out++) = 0xE0 + (tlen >> 2) - 1;

			memcpy(out, rptr, tlen);

			rptr += tlen;
			out += tlen;
		}

		if (bcost == 2)
		{
			// two byte long form
			*(out++) = ((boffset >> 8) << 5) + ((blen - 3) << 2) + run;
			*(out++) = boffset;
		}
		else if (bcost == 3)
		{
			// three byte long form
			*(out++) = 0x80 + (blen - 4);
			*(out++) = (run << 6) + (boffset >> 8);
			*(out++) = boffset;
		}
		else
		{
			// four byte very long form
			*(out++) = 0xC0 + ((boffset >> 16) << 4) + (((blen - 5) >> 8) << 2) + run;
			*(out++) = (boffset >> 8);
			*(out++) = boffset;
			*(out++) = blen - 5;
		}

		if (run)
		{
			memcpy(out, rptr, run);
			out += run;
			run = 0;
		}

		if (fast)
		{
			hoffset = cptr - in;
			link[hoffset & kRefPackWindowMask] = hashtbl[hash];
			hashtbl[hash] = hoffset;
			cptr += blen;
		}
		else
		{
			for (int i = 0; i < blen; i++)
			{
				hash = refhash(cptr);
				hoffset = cptr - in;
				link[hoffset & kRefPackWindowMask] = hashtbl[hash];
				hashtbl[hash] = hoffset;
				++cptr;
			}
		}

		rptr = cptr;
		inSize -= blen;
	}

	while (run > 3)
	{
		// no match at end, use literal
		tlen = std::min<uint32_t>(112, run & (~3));
		run -= tlen;
		*(out++) = 0xE0 + (tlen >> 2) - 1;

		memcpy(out, rptr, tlen);

		rptr += tlen;
		out += tlen;
	}

	// end of stream command + 0..3 literal
	*out++ = 0xFC + run;
	if (run)
	{
		memcpy(out, rptr, run);
		out += run;
	}

	free(hashtbl);
	free(link);

	return out - out_orig;
}

static uint32_t encoderef(uint8_t* in, uint32_t inSize, uint8_t* out, int const* flags)
{
	int fastCompressionFlag = flags[0];
	encodeintref(out, 0x10FB, 2);
	encodeintref(&out[2], inSize, 3);
	return refpack(in, inSize, &out[5], 0x20000, fastCompressionFlag & 1);
}

static uint32_t decoderef(uint8_t const* in, uint32_t* bytesReadPtr, uint8_t* out)
{
	uint32_t decompressedSize = 0;
	uint8_t const* cursor = in;
	uint8_t const* ref;
	uint8_t first, second, third, fourth, run;

	if (in != NULL)
	{
		cursor = in + 2;
		if ((in[0] & 1) != 0)
		{
			cursor = in + 5;
		}

		decompressedSize = (cursor[0] << 16) | (cursor[1] << 8) | cursor[2];
		cursor += 3;

		for (;;)
		{
			first = *(cursor++);
			if ((first & 0x80) == 0)
			{
				// short form
				second = *(cursor++);
				run = first & 3;

				while (run--)
				{
					*(out++) = *(cursor++);
				}

				ref = out - 1 - (((first & 0x60) << 3) + second);
				run = ((first & 0x1C) >> 2) + 3 - 1;

				do
				{
					*(out++) = *(ref++);
				}
				while (run--);
			}
			else if ((first & 0x40) == 0)
			{
				// long form
				second = *(cursor++);
				third = *(cursor++);
				run = second >> 6;

				while (run--)
				{
					*(out++) = *(cursor++);
				}

				ref = out - 1 - (((second & 0x3F) << 8) + third);
				run = (first & 0x3F) + 4 - 1;

				do
				{
					*(out++) = *(ref++);
				}
				while (run--);
			}
			else if ((first & 0x20) == 0)
			{
				// very long form
				second = *(cursor++);
				third = *(cursor++);
				fourth = *(cursor++);
				run = first & 3;

				while (run--)
				{
					*(out++) = *(cursor++);
				}

				ref = out - 1 - (((first & 0x10) >> 4) << 16) + (second << 8) + third;
				run = (((first & 0x0C) >> 2) << 8) + fourth + 5 - 1;

				do
				{
					*(out++) = *(ref++);
				}
				while (run--);
			}
			else if ((run = ((first & 0x1F) << 2) + 4) <= 112)
			{
				// literal
				while (run--)
				{
					*(out++) = *(cursor++);
				}

				continue;
			}
			else
			{
				// EOF (+0..3 literal)
				run = first & 3;
				while (run--)
				{
					*(out++) = *(cursor++);
				}

				break;
			}
		}
	}

	if (bytesReadPtr != NULL)
	{
		*bytesReadPtr = cursor - in;
	}

	return decompressedSize;
}

bool cRZFastCompression3::CompressData(void const* data, uint32_t size, uint8_t* compressedData, uint32_t& compressedSize)
{
	int flag = quickCompressionEnabled;
	uint32_t result = encoderef(reinterpret_cast<uint8_t*>(const_cast<void*>(data)), size, &compressedData[4], &flag);

	compressedSize = result + 4;
	// FUTURE: adjust on big endian systems
	*reinterpret_cast<uint32_t*>(compressedData) = result + 4;

	return result != 0;
}

bool cRZFastCompression3::DecompressData(uint8_t const* data, uint32_t size, void* decompressedData, uint32_t& decompressedSize)
{
	uint32_t bytesRead = size;
	uint32_t result = decoderef(&data[4], &bytesRead, reinterpret_cast<uint8_t*>(decompressedData));
	decompressedSize = result;

	if (result == 0 && GetLengthOfDecompressedData(data) != 0)
	{
		return false;
	}

	return true;
}