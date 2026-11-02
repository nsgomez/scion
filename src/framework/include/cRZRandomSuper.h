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

#pragma once
#include <stdint.h>

const uint32_t kMT19937ArrayLength = 624;
const uint32_t kMT19937ArrayMidpoint = 397;

class cRZRandomSuper
{
public:
	cRZRandomSuper(uint32_t startSeed) :
		mArrayPtr(NULL),
		mArrayCount(kMT19937ArrayLength)
	{
		Seed(startSeed);
	}

	void Seed(uint32_t newSeed);
	void Reload();

	double RandomDoubleRangeUniform(double lower, double upper);
	double RandomDoubleUniform();

	int32_t RandomSint32RangeUniform(int32_t lower, int32_t upper);

	uint32_t RandomUint32Uniform(uint32_t upper);
	uint32_t RandomUint32Uniform();
	uint32_t RandomUint32WeightedChoice(uint32_t count, double const* weights);

protected:
	uint32_t Hash(int32_t first, int32_t second);

	uint32_t mArray[kMT19937ArrayLength];
	uint32_t* mArrayPtr;
	int32_t mArrayCount;
};