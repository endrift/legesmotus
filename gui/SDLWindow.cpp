/*
 * gui/SDLWindow.cpp
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

#include "SDLWindow.hpp"
#include "common/misc.hpp"

using namespace LM;
using namespace std;

SDLWindow::SDLWindow(int width, int height, int depth, int flags) : Window(width, height, depth, flags) {
	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		SDL_InitSubSystem(SDL_INIT_VIDEO);
	}

	int rflags = SDL_HWSURFACE|SDL_OPENGL;
	if (is_fullscreen()) {
		rflags |= SDL_FULLSCREEN;
	}
	depth = SDL_VideoModeOK(width, height, depth, rflags);
	if (depth == 0) {
		throw VmodeNotSupportedException("Video mode not supported.\n"
			"If this problem persists, try deleting the settings file for Leges Motus.",
			width, height, depth, is_fullscreen());
	}

	if (depth != get_depth()) {
		set_depth(depth);
	}
	
	switch (depth) {
	case 16:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
		break;
	case 24:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		break;
	case 32:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		break;
	default:
		throw Exception("Bad depth");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#if SDL_VERSION_ATLEAST(1, 2, 10)
	// Enable VSYNC
	if (flags & FLAG_VSYNC) {
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // Deprecated in SDL 1.3
	}
#endif

	SDL_WM_SetCaption("Leges Motus","Leges Motus");
	//SDL_ShowCursor(SDL_DISABLE);
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_Surface* screen = SDL_SetVideoMode(width, height, depth, rflags);
	if (screen == NULL) {
		throw Exception(SDL_GetError());
	}

	m_context = new GLESContext(width, height);
}

SDLWindow::~SDLWindow() {
	delete m_context;
}

bool SDLWindow::rebuild(int width, int height, int depth, int flags) {
	return false;
}

void SDLWindow::set_icon_internal(Image* icon) {
	STUB(SDLWindow::set_icon_internal);
}

SDLWindow* SDLWindow::get_instance(int width, int height, int depth, int flags) {
	SDLWindow* instance = new SDLWindow(width, height, depth, flags);
	set_instance(instance);
	return instance;
}

SDLWindow* SDLWindow::get_optimal_instance(int flags) {
	STUB(SDLWindow::get_optimal_instance);
	return NULL;
}

GLESContext* SDLWindow::get_context() {
	return m_context;
}

void SDLWindow::redraw() {
	get_context()->redraw();

	SDL_GL_SwapBuffers();
}
