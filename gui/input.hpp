/*
 * gui/input.hpp
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

#ifndef LM_GUI_INPUT_HPP
#define LM_GUI_INPUT_HPP

namespace LM {
	enum KeyType {
		KEY_LETTER = 0,
		KEY_OTHER,
		KEY_NUMPAD,
		KEY_ESCAPE,
		KEY_UP,
		KEY_RIGHT,
		KEY_DOWN,
		KEY_LEFT,
		KEY_L_SHIFT,
		KEY_R_SHIFT,
		KEY_L_CONTROL,
		KEY_R_CONTROL,
		KEY_L_ALT,
		KEY_R_ALT,
		KEY_L_META,
		KEY_R_META,
		KEY_L_SUPER,
		KEY_R_SUPER,
		KEY_MENU,
		KEY_BACKSPACE,
		KEY_TAB,
		KEY_ENTER,
		KEY_INSERT,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PGUP,
		KEY_PGDN,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_F13,
		KEY_F14,
		KEY_F15
	};

	enum KeyModifier {
		MOD_SHIFT = 1,
		MOD_CONTROL = 2,
		MOD_ALT = 4,
		MOD_META = 8
	};

	enum MouseButton {
		BUTTON_LEFT = 1,
		BUTTON_MIDDLE = 2,
		BUTTON_RIGHT = 3,
		BUTTON_WHEEL_LEFT = 6,
		BUTTON_WHEEL_RIGHT = 7,
		BUTTON_WHEEL_UP = 4,
		BUTTON_WHEEL_DOWN = 5
	};

	struct KeyEvent {
		wchar_t character;
		KeyType type;
		long raw;
		int modifiers;
		bool down;
	};

	struct MouseMotionEvent {
		int x, y;
		int dx, dy;
	};

	struct MouseButtonEvent {
		int button;
		bool down;
	};

	inline int mouse_button_bit(MouseButton button) {
		return 1<<(button - 1);
	}
	
	const wchar_t* get_key_name(const KeyEvent& key);
}

#endif
