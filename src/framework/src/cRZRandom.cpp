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

#include <cmath>
#include <ctime>
#include "cRZRandom.h"

cRZRandom::cRZRandom() :
	integerSeed(0),
	doubleSeed(0)
{
}

cRZRandom::cRZRandom(uint32_t startSeed) :
	integerSeed(startSeed),
	doubleSeed(startSeed)
{
}

void cRZRandom::Seed(uint32_t newSeed)
{
	if (newSeed == 0xFFFFFFFF)
	{
		uint32_t wallTime = time(NULL);
		uint32_t clockTime = clock();

		newSeed = (clockTime << 16 | clockTime) ^ wallTime;
	}

	integerSeed = newSeed;
	doubleSeed = newSeed;
}

double cRZRandom::RandomDoubleGaussian()
{
	double x1, x2;
	double w;
	double output;

	if (!hasSavedGaussianNoise)
	{
		do
		{
			do
			{
				x1 = (RandomDoubleUniform() * 2.0) - 1.0;
				x2 = (RandomDoubleUniform() * 2.0) - 1.0;
				w = (x1 * x1) + (x2 * x2);
			}
			while (w >= 1.0);
		}
		while (w == 0.0);

		w = sqrt((-2.0 * log(w)) / w);

		savedGaussianNoise = x1 * w;
		hasSavedGaussianNoise = true;

		output = x2 * w;
	}
	else
	{
		hasSavedGaussianNoise = false;
		output = savedGaussianNoise;
	}

	return w;
}

double cRZRandom::RandomDoubleRangeGaussian(double lower, double upper)
{
	double x = RandomDoubleGaussian();
	double multiplier = x + 3.0;

	if (multiplier >= 6.0)
	{
		multiplier = 5.9999;
	}

	if (multiplier < 0.0)
	{
		multiplier = 0.0;
	}

	return multiplier * (1.0 / 6.0) * (upper - lower) + lower;
}

double cRZRandom::RandomDoubleRangeGaussianFast(double lower, double upper)
{
	double x = RandomDoubleUniform();
	double y = RandomDoubleUniform();
	double z = RandomDoubleUniform();

	return (x + y + z) * (upper - lower) * (1.0/3.0) + lower;
}

double cRZRandom::RandomDoubleRangeUniform(double lower, double upper)
{
	return (upper - lower) * RandomDoubleUniform() + lower;
}

double cRZRandom::RandomDoubleUniform()
{
	uint32_t seed = doubleSeed;
	if (seed == 0)
	{
		doubleSeed = seed = clock();
	}

	doubleSeed = seed * 0x278dde6d;
	return (double)(seed * 0x278dde6d) * 2.32830643653870E-10 + 0.5;
}

int32_t cRZRandom::RandomSint32RangeGaussianFast(int32_t lower, int32_t upper)
{
	uint32_t t0 = RandomUint32Uniform();
	uint32_t t1 = RandomUint32Uniform();
	uint32_t t2 = RandomUint32Uniform();
	uint32_t t3 = RandomUint32Uniform();
	uint64_t tcubic = ((((uint64_t)t0 + t1) + ((uint64_t)t2 + t3) + 2) >> 2);

	return lower + (int32_t)((tcubic * (uint32_t)(upper - lower)) >> 32);
}

int32_t cRZRandom::RandomSint32RangeUniform(int32_t lower, int32_t upper)
{
	int32_t result = 0;

	if (upper - lower != 0)
	{
		result = (int32_t)RandomUint32Uniform((uint32_t)(upper - lower));
	}

	return result + lower;
}

uint32_t cRZRandom::RandomUint32Uniform(uint32_t upper)
{
	uint32_t result = 0;
	if (upper != 0)
	{
		result = (uint32_t)(((uint64_t)RandomUint32Uniform() * (uint64_t)upper) >> 32);
	}

	return result;
}

uint32_t cRZRandom::RandomUint32Uniform()
{
	uint64_t result64 = integerSeed * (uint64_t)1103515245 + 12345;
	integerSeed = (uint32_t)result64;
	return (uint32_t)(result64 >> 16);
}