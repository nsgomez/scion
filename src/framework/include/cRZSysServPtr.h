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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "cIGZFramework.h"
#include "RZStatics.h"

template <class T, GZIID iid, GZGUID srvid>
class cRZSysServPtr
{
public:
	cRZSysServPtr()
	{
		cIGZFramework* framework = RZGetFramework();
		if (framework != NULL)
		{
			framework->GetSystemService(srvid, iid, reinterpret_cast<void**>(&srv));
		}
	}

	~cRZSysServPtr()
	{
		if (srv != NULL)
		{
			srv->Release();
		}
	}

	T* operator->() const { return srv; }
	T& operator*() const { return srv; }
	operator T*() const { return srv; }

protected:
	T* srv;
};