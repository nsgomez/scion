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
#include <list>
#include <stdint.h>

static const uint32_t kShiftModifier    = (1 << 0);
static const uint32_t kControlModifier  = (1 << 1);
static const uint32_t kMenuModifier     = (1 << 2);

class cRZKeyboard
{
public:
	cRZKeyboard();

	virtual void ClearKeyboard();
	virtual bool IsKeyDown(uint32_t key);
	virtual bool IsKeyDown(uint32_t key, uint32_t modifiers);
	virtual bool IsKeyDownNow(uint32_t key);
	virtual bool CharIsModifier(uint32_t key);
	virtual uint32_t GetCurrentModifierState();
	virtual bool IsToggleKeySet(uint32_t key);
	virtual void ProcessKeyDown(uint32_t key);
	virtual void ProcessKeyUp(uint32_t key);

	static bool IsKeyDownNowStatic(uint32_t key);
	static bool IsToggleKeySetStatic(uint32_t key);

protected:
	std::list<uint32_t> keysPressed;
	bool keyStates[256];
};