/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
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

#include <ctime>
#include "cRZRandomSuper.h"

static const uint32_t kMT19937Coefficient = 0x9908b0df;
static const uint32_t kMT19937InitMultiplier = 69069;
static const uint32_t kMT19937InitIncrement = 69070;

void cRZRandomSuper::Seed(uint32_t newSeed)
{
	uint32_t* it = mArray;
	if (newSeed == 0xffffffff)
	{
		newSeed = Hash(time(NULL), clock());
	}

	uint32_t value = newSeed | 1;
	for (int i = 0; i < kMT19937ArrayLength; i++)
	{
		*it = value & 0xffff0000;
		value *= kMT19937InitMultiplier;
		*it |= value >> 16;

		++it;
		value = value * kMT19937InitMultiplier + kMT19937InitIncrement;
	}

	Reload();
}

void cRZRandomSuper::Reload()
{
	const uint32_t kUpperMask = 0x80000000;
	const uint32_t kLowerMask = 0x7fffffff;
	const uint32_t kMT19937Vector = 0x9908b0df;

	uint32_t* tail = mArray;
	uint32_t* head = mArray + 1;

	uint32_t prev = *tail;
	uint32_t next = *head;

	for (int i = kMT19937ArrayLength - kMT19937ArrayMidpoint; i > 0; i--)
	{
		uint32_t y = (prev & kUpperMask) | (next & kLowerMask);
		*tail = *(tail + kMT19937ArrayMidpoint) ^ (y >> 1) ^ ((next & 1) ? kMT19937Vector : 0);

		++tail;
		prev = next;
		next = *++head;
	}

	uint32_t* it = mArray;
	for (int i = kMT19937ArrayMidpoint - 1; i > 0; i--)
	{
		uint32_t y = (prev & kUpperMask) | (next & kLowerMask);
		*tail = *tail ^ (y >> 1) ^ ((next & 1) ? kMT19937Vector : 0);

		++tail;
		++it;

		prev = next;
		next = *++head;
	}

	uint32_t y = (prev & kUpperMask) | (mArray[0] & kLowerMask);
	*tail = *it ^ (y >> 1) ^ ((mArray[0] & 1) ? kMT19937Vector : 0);

	mArrayCount = kMT19937ArrayLength;
	mArrayPtr = mArray;
}

uint32_t cRZRandomSuper::Hash(int32_t first, int32_t second)
{
	static uint32_t incrementor = 0;

	uint8_t const* firstBytes = reinterpret_cast<uint8_t const*>(&first);
	uint8_t const* secondBytes = reinterpret_cast<uint8_t const*>(&second);

	uint32_t firstHash = 0;
	uint32_t secondHash = 0;

	for (int i = 0; i < 4; i++)
	{
		firstHash = firstHash * 0x101 + firstBytes[i];
	}

	for (int i = 0; i < 4; i++)
	{
		secondHash = secondHash * 0x101 + secondBytes[i];
	}

	uint32_t result = (incrementor + firstHash) ^ secondHash;
	incrementor++;

	return result;
}

uint32_t cRZRandomSuper::RandomUint32Uniform()
{
	if (--mArrayCount < 0)
	{
		Reload();
		--mArrayCount;
	}

	uint32_t y = *mArrayPtr++;
	y ^= y >> 11;
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= y >> 18;

	return y;
}

uint32_t cRZRandomSuper::RandomUint32Uniform(uint32_t upper)
{
	return (uint32_t)(((uint64_t)RandomUint32Uniform() * upper) >> 32);
}

int32_t cRZRandomSuper::RandomSint32RangeUniform(int32_t lower, int32_t upper)
{
	return (int32_t)RandomUint32Uniform((uint32_t)(upper - lower)) + lower;
}

double cRZRandomSuper::RandomDoubleUniform()
{
	return (double)((int32_t)RandomUint32Uniform()) * 2.3283064365386963e-10 + 0.5;
}

double cRZRandomSuper::RandomDoubleRangeUniform(double lower, double upper)
{
	return (upper - lower) * RandomDoubleUniform() + lower;
}

uint32_t cRZRandomSuper::RandomUint32WeightedChoice(uint32_t count, double const* weights)
{
	if (count < 2)
	{
		return 0;
	}

	double total = 0.0;
	for (uint32_t i = 0; i < count; i++)
	{
		double weight = weights[i];
		if (!(weight <= 0.0))
		{
			total += weight;
		}
	}

	if (total == 0.0)
	{
		return RandomUint32Uniform(count);
	}

	double accumulator = RandomDoubleRangeUniform(0.0, total);
	for (uint32_t i = 0; i < count; i++)
	{
		double weight = weights[i];
		if (!(weight <= 0.0))
		{
			if (accumulator < weight)
			{
				return i;
			}

			accumulator -= weight;
		}
	}

	return count - 1;
}