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

#pragma once
#include <stdint.h>

class cRZRandom
{
public:
	cRZRandom();
	cRZRandom(uint32_t startSeed);

	void Seed(uint32_t newSeed);

	double RandomDoubleGaussian();
	double RandomDoubleRangeGaussian(double lower, double upper);
	double RandomDoubleRangeGaussianFast(double lower, double upper);
	double RandomDoubleRangeUniform(double lower, double upper);
	double RandomDoubleUniform();

	int32_t RandomSint32RangeGaussianFast(int32_t lower, int32_t upper);
	int32_t RandomSint32RangeUniform(int32_t lower, int32_t upper);

	uint32_t RandomUint32Uniform(uint32_t upper);
	uint32_t RandomUint32Uniform();

protected:
	uint32_t integerSeed;
	uint32_t doubleSeed;
	bool hasSavedGaussianNoise;
	double savedGaussianNoise;
};