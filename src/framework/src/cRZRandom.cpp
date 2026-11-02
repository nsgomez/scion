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

#include <cmath>
#include <ctime>
#include "cRZRandom.h"

cRZRandom::cRZRandom(uint32_t startSeed)
{
	Seed(startSeed);
	mHasSavedGaussianNoise = false;
}

void cRZRandom::Seed(uint32_t newSeed)
{
	if (newSeed == 0xFFFFFFFF)
	{
		uint32_t wallTime = time(NULL);
		uint32_t clockTime = clock();

		newSeed = (clockTime << 16 | clockTime) ^ wallTime;
	}

	mIntegerSeed = newSeed;
	mDoubleSeed = newSeed;
}

double cRZRandom::RandomDoubleGaussian()
{
	double x1, x2;
	double w;
	double output;

	if (!mHasSavedGaussianNoise)
	{
		do
		{
			do
			{
				x1 = (RandomDoubleUniform() * 2.0) - 1.0;
				x2 = (RandomDoubleUniform() * 2.0) - 1.0;
				w = (x2 * x2) + (x1 * x1);
			}
			while (w >= 1.0);
		}
		while (w == 0.0);

		w = sqrt((log(w) / w) * -2.0);

		mSavedGaussianNoise = x1 * w;
		mHasSavedGaussianNoise = true;

		output = w * x2;
	}
	else
	{
		mHasSavedGaussianNoise = false;
		output = mSavedGaussianNoise;
	}

	return output;
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

	return (upper - lower) * (multiplier * (1.0 / 6.0)) + lower;
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
	if (mDoubleSeed == 0)
	{
		mDoubleSeed = (uint32_t)clock();
	}

	int32_t value = (int32_t)(mDoubleSeed * 0x278dde6d);
	mDoubleSeed = (uint32_t)value;

	return (double)value * 2.32830643653870E-10 + 0.5;
}

int32_t cRZRandom::RandomSint32RangeGaussianFast(int32_t lower, int32_t upper)
{
	uint32_t t0 = RandomUint32Uniform();
	uint32_t t1 = RandomUint32Uniform();
	uint32_t t2 = RandomUint32Uniform();
	uint32_t t3 = RandomUint32Uniform();
	uint64_t sum = ((uint64_t)t3 + t2 + t1 + t0 + 2) >> 2;
	uint64_t product = (uint64_t)(sum * (uint32_t)(upper - lower));

	return (int32_t)(product >> 32) + lower;
}

int32_t cRZRandom::RandomSint32RangeUniform(int32_t lower, int32_t upper)
{
	return (int32_t)RandomUint32Uniform((uint32_t)(upper - lower)) + lower;
}

uint32_t cRZRandom::RandomUint32Uniform(uint32_t upper)
{
	if (upper == 0)
	{
		return 0;
	}

	return (uint32_t)(((uint64_t)RandomUint32Uniform() * (uint64_t)upper) >> 32);
}

uint32_t cRZRandom::RandomUint32Uniform()
{
	uint64_t result64 = mIntegerSeed * (uint64_t)1103515245 + 12345;
	mIntegerSeed = (uint32_t)result64;
	return (uint32_t)(result64 >> 16);
}