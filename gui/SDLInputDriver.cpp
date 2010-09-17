/*
 * gui/SDLInputDriver.cpp
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

#include "SDLInputDriver.hpp"
#include <SDL/SDL.h>

using namespace LM;
using namespace std;

#define KEYMAP(sdlk, lmk) \
	case SDLK_ ## sdlk: \
		ke.type = KEY_ ## lmk; \
		break

SDLInputDriver::SDLInputDriver() {
	SDL_EnableUNICODE(true);
}

int SDLInputDriver::update() {
	int updates = 0;
	SDL_Event e;
	SDL_keysym *ks = &e.key.keysym;

	KeyEvent ke;
	MouseMotionEvent mme;
	MouseButtonEvent mbe;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			ke.down = e.key.state == SDL_PRESSED;
			ke.character = ks->unicode;
			ke.modifiers = 0;
			ke.modifiers |= (ks->mod & KMOD_SHIFT)?MOD_SHIFT:0;
			ke.modifiers |= (ks->mod & KMOD_CTRL)?MOD_CONTROL:0;
			ke.modifiers |= (ks->mod & KMOD_ALT)?MOD_ALT:0;
			ke.modifiers |= (ks->mod & KMOD_META)?MOD_META:0;
			ke.raw = ks->sym;

			if (ks->sym >= SDLK_KP0 && ks->sym <= SDLK_KP_EQUALS) {
				ke.type = KEY_NUMPAD;
			} else {
				switch (ks->sym) {
					KEYMAP(ESCAPE, ESCAPE);
					KEYMAP(BACKSPACE, BACKSPACE);
					KEYMAP(TAB, TAB);
					KEYMAP(CLEAR, OTHER);
					KEYMAP(RETURN, ENTER);
					KEYMAP(PAUSE, OTHER);
					KEYMAP(INSERT, INSERT);
					KEYMAP(DELETE, DELETE);
					KEYMAP(HOME, HOME);
					KEYMAP(END, END);
					KEYMAP(PAGEUP, PGUP);
					KEYMAP(PAGEDOWN, PGDN);
					KEYMAP(RSHIFT, R_SHIFT);
					KEYMAP(LSHIFT, L_SHIFT);
					KEYMAP(RCTRL, R_CONTROL);
					KEYMAP(LCTRL, L_CONTROL);
					KEYMAP(RALT, R_ALT);
					KEYMAP(LALT, L_ALT);
					KEYMAP(RMETA, R_META);
					KEYMAP(LMETA, L_META);
					KEYMAP(UP, UP);
					KEYMAP(RIGHT, RIGHT);
					KEYMAP(DOWN, DOWN);
					KEYMAP(LEFT, LEFT);
					KEYMAP(F1, F1);
					KEYMAP(F2, F2);
					KEYMAP(F3, F3);
					KEYMAP(F4, F4);
					KEYMAP(F5, F5);
					KEYMAP(F6, F6);
					KEYMAP(F7, F7);
					KEYMAP(F8, F8);
					KEYMAP(F9, F9);
					KEYMAP(F10, F10);
					KEYMAP(F11, F11);
					KEYMAP(F12, F12);
					KEYMAP(F13, F13);
					KEYMAP(F14, F14);
					KEYMAP(F15, F15);

					default:
						ke.type = KEY_LETTER;
						break;
				}
			}

			register_event(ke);
			++updates;
			break;

		case SDL_MOUSEMOTION:
			mme.x = e.motion.x;
			mme.y = e.motion.y;
			mme.dx = e.motion.xrel;
			mme.dy = e.motion.yrel;
			register_event(mme);
			++updates;
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mbe.down = e.button.state == SDL_PRESSED;
			mbe.button = e.button.button;
			register_event(mbe);
			++updates;
			break;
		}
	}

	return updates;
}

Point SDLInputDriver::mouse_position() const {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return Point(x, y);
}

int SDLInputDriver::mouse_buttons() const {
	return SDL_GetMouseState(NULL, NULL);
}
