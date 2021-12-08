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

template<class T>
class cRZAutoRefCount
{
public:
	cRZAutoRefCount() : obj(NULL) { }
	cRZAutoRefCount(T* other) : obj(other)
	{
		if (other)
		{
			other->AddRef();
		}
	}

	virtual ~cRZAutoRefCount()
	{
		if (obj)
		{
			obj->Release();
		}
	}

	cRZAutoRefCount<T>& operator=(T* other)
	{
		if (obj != other)
		{
			if (obj != NULL)
			{
				obj->Release();
			}

			obj = other;

			if (obj != NULL)
			{
				obj->AddRef();
			}
		}

		return *this;
	}

	T* operator->() const
	{
		return obj;
	}

	T& operator*() const
	{
		return *obj;
	}

	operator T*() const
	{
		return obj;
	}

protected:
	T* obj;
};