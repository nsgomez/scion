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
	cRZAutoRefCount() : obj(NULL)
	{
		if (obj)
		{
			obj->AddRef();
		}
	}

	cRZAutoRefCount(cRZAutoRefCount const& other)
	{
		obj = other.obj;
		if (obj)
		{
			obj->AddRef();
		}
	}

	cRZAutoRefCount(T* other) : obj(other)
	{
		if (obj)
		{
			obj->AddRef();
		}
	}

	~cRZAutoRefCount()
	{
		if (obj)
		{
			obj->Release();
		}
	}

	cRZAutoRefCount<T>& operator=(T* other)
	{
		T* objToReplace = obj;
		if (other != objToReplace)
		{
			if (other != NULL)
			{
				other->AddRef();
			}

			obj = other;

			if (objToReplace != NULL)
			{
				objToReplace->Release();
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

	bool operator==(T* const other) const
	{
		return obj == other;
	}

	bool operator==(cRZAutoRefCount<T> const& other) const
	{
		return obj == other.obj;
	}

protected:
	T* obj;
};