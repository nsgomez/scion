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

#include "cRZNullIStream.h"

cRZNullIStream::cRZNullIStream() :
	cRZIStream(false)
{
}

cRZNullIStream::~cRZNullIStream()
{
}

bool cRZNullIStream::Skip(uint32_t bytes)
{
	hasError = true;
	return false;
}

bool cRZNullIStream::GetVoid(void* buffer, uint32_t size)
{
	hasError = true;
	return false;
}

bool cRZNullIStream::GetError() const
{
	return hasError;
}