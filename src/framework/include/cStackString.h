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

namespace
{
	template <uint32_t N>
	class cStackString
	{
	public:
		cStackString()
		{
			mHeap = NULL;
			mBegin = mStorage;
			mEnd = mStorage;
			mCap = mStorage + N;
		}

		~cStackString()
		{
			delete[] mHeap;
		}

		char* begin() const { return mBegin; }
		char* end() const { return mEnd; }
		char* cap() const { return mCap; }

		void reserve(uint32_t count);
		void resize(uint32_t count);
		void push_back(char const& ch);
		void append(char const* src, uint32_t count);
		void append(uint32_t count, char ch);

	private:
		char* mBegin;
		char* mEnd;
		char* mCap;
		char* mHeap;
		char mStorage[N];
	};

	template <uint32_t N>
	void cStackString<N>::reserve(uint32_t count)
	{
		if (count < (uint32_t)(mCap - mBegin))
		{
			return;
		}

		char* buf = new char[count];
		uint32_t curSize = (uint32_t)(mEnd - mBegin);
		memmove(buf, mBegin, curSize);

		delete[] mHeap;

		mHeap = buf;
		mBegin = buf;
		mCap = buf + count;
		mEnd = buf + curSize;
	}

	template <uint32_t N>
	void cStackString<N>::resize(uint32_t count)
	{
		if (count > (uint32_t)(mEnd - mBegin))
		{
			reserve(count);
		}

		mEnd = mBegin + count;
	}

	template <uint32_t N>
	void cStackString<N>::push_back(char const& ch)
	{
		if (mEnd == mCap)
		{
			reserve(2u * (uint32_t)(mCap - mBegin));
			*mEnd = ch;
			++mEnd;
		}
	}

	template <uint32_t N>
	void cStackString<N>::append(char const* src, uint32_t count)
	{
		if ((uint32_t)(mCap - mEnd) < count)
		{
			reserve(std::max(
				2u * (uint32_t)(mCap - mBegin),
				(mEnd - mBegin) + count));
		}

		memmove(mEnd, src, count);
		mEnd += count;
	}

	template <uint32_t N>
	void cStackString<N>::append(uint32_t count, char ch)
	{
		if ((uint32_t)(mCap - mEnd) < count)
		{
			reserve(std::max(
				2u * (uint32_t)(mCap - mBegin),
				(mEnd - mBegin) + count));
		}

		std::fill(mEnd, mEnd + count, ch);
		mEnd += count;
	}
}