/*
 * gui/input.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "input.hpp"

#include <cwchar>

using namespace LM;
using namespace std;

#define KEYTYPE_NAME(lmk, name) \
	case KEY_ ## lmk: \
		return (L ## name)

const wchar_t* LM::get_key_name(const KeyEvent& key) {
	static wchar_t npk[] = L"Numpad X";
	static wchar_t k[] = L"X";

	switch (key.type) {
		KEYTYPE_NAME(ESCAPE, "Esc");
		KEYTYPE_NAME(UP, "Up");
		KEYTYPE_NAME(RIGHT, "Right");
		KEYTYPE_NAME(DOWN, "Down");
		KEYTYPE_NAME(LEFT, "Left");
		KEYTYPE_NAME(L_SHIFT, "Shift (L)");
		KEYTYPE_NAME(R_SHIFT, "Shift (R)");
		KEYTYPE_NAME(L_CONTROL, "Ctrl (L)");
		KEYTYPE_NAME(R_CONTROL, "Ctrl (R)");
		KEYTYPE_NAME(L_ALT, "Alt (L)");
		KEYTYPE_NAME(R_ALT, "Alt (R)");
		KEYTYPE_NAME(L_META, "Meta (L)");
		KEYTYPE_NAME(R_META, "Meta (R)");
		KEYTYPE_NAME(BACKSPACE, "Backspace");
		KEYTYPE_NAME(TAB, "Tab");
		KEYTYPE_NAME(ENTER, "Enter");
		KEYTYPE_NAME(INSERT, "Insert");
		KEYTYPE_NAME(DELETE, "Delete");
		KEYTYPE_NAME(HOME, "Home");
		KEYTYPE_NAME(END, "End");
		KEYTYPE_NAME(PGUP, "Page Up");
		KEYTYPE_NAME(PGDN, "Page Down");
		KEYTYPE_NAME(F1, "F1");
		KEYTYPE_NAME(F2, "F2");
		KEYTYPE_NAME(F3, "F3");
		KEYTYPE_NAME(F4, "F4");
		KEYTYPE_NAME(F5, "F5");
		KEYTYPE_NAME(F6, "F6");
		KEYTYPE_NAME(F7, "F7");
		KEYTYPE_NAME(F8, "F8");
		KEYTYPE_NAME(F9, "F9");
		KEYTYPE_NAME(F10, "F10");
		KEYTYPE_NAME(F11, "F11");
		KEYTYPE_NAME(F12, "F12");
		KEYTYPE_NAME(F13, "F13");
		KEYTYPE_NAME(F14, "F14");
		KEYTYPE_NAME(F15, "F15");

		case KEY_NUMPAD:
			if (key.character == L'\r') {
				return L"Numpad Enter";
			}
			npk[wcslen(npk) - 1] = key.character;
			return npk;

		case KEY_LETTER:
			if (key.character) {
				k[wcslen(k) - 1] = key.character;
				return k;
			} else if (!key.down) {
				return L"Letter released";
			}

		default:
			return L"(Unknown)";
	}
}
