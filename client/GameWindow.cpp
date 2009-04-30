/*
 * GameWindow.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "common/LMException.hpp"
#include "GameWindow.hpp"
#include "compat_gl.h"
#include <cmath>

using namespace std;

GameWindow* GameWindow::m_instance = NULL;

GameWindow::GameWindow(int width, int height, int depth, bool fullscreen) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_fullscreen = fullscreen;
	m_offset_x = 0;
	m_offset_y = 0;
	switch (depth) {
	case 24:
		SDL_InitSubSystem(SDL_INIT_VIDEO);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		break;
	case 32:
		SDL_InitSubSystem(SDL_INIT_VIDEO);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
		break;
	default:
		throw LMException("Bad depth");
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#if SDL_MAJOR_VERSION == 1 && SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL >= 10
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // Deprecated in SDL 1.3
#endif
	SDL_ShowCursor(SDL_DISABLE);
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	m_context = SDL_SetVideoMode(width, height, depth, flags);
	if (m_context == NULL) {
		throw LMException(SDL_GetError());
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glViewport(0,0,width,height);
	glOrtho(0,width,height,0,1,-1);
}

GameWindow::~GameWindow() {
	if (m_context != NULL) {
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	m_instance = NULL;
}

GameWindow* GameWindow::get_instance(int width, int height, int depth, bool fullscreen) {
	if (m_instance == NULL) {
		m_instance = new GameWindow(width, height, depth, fullscreen);
	} else {
		m_instance->set_dimensions(width,height);
		m_instance->set_fullscreen(fullscreen);
	}
	return m_instance;
}

GameWindow* GameWindow::get_instance() {
	return m_instance;
}

void GameWindow::destroy_instance() {
	if (m_instance == NULL) {
		return;
	}
	delete m_instance;
	m_instance = NULL;
}

int GameWindow::get_width() const {
	return m_width;
}

int GameWindow::get_height() const {
	return m_height;
}

double GameWindow::get_offset_x() const {
	return m_offset_x;
}

double GameWindow::get_offset_y() const {
	return m_offset_y;
}

void GameWindow::set_dimensions(int width, int height) {
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (m_fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_Surface* context = SDL_SetVideoMode(width, height, m_depth, flags);
	if (context != NULL) {
		m_context = context;
		m_width = width;
		m_height = height;
	}
}

void GameWindow::set_fullscreen(bool fullscreen) {
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_Surface* context = SDL_SetVideoMode(m_width, m_height, m_depth, flags);
	if(m_context != NULL) {
		m_context = context;
		m_fullscreen = fullscreen;
	}
}

bool GameWindow::is_fullscreen() const {
	return m_fullscreen;
}

void GameWindow::set_offset_x(double offset) {
	m_offset_x = offset;
}

void GameWindow::set_offset_y(double offset) {
	m_offset_y = offset;
}

void GameWindow::register_graphic(Graphic* graphic) {
	for (list<Graphic*>::iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_graphics.insert(iter,graphic);
			return;
		}
	}
	m_graphics.push_back(graphic);
}

void GameWindow::unregister_graphic(Graphic* graphic) {
	m_graphics.remove(graphic);
}


void GameWindow::register_hud_graphic(Graphic* graphic) {
	for (list<Graphic*>::iterator iter = m_hud_graphics.begin(); iter != m_hud_graphics.end(); ++iter) {
		if ((*iter)->get_priority() < graphic->get_priority()) {
			m_hud_graphics.insert(iter,graphic);
			return;
		}
	}
	m_hud_graphics.push_back(graphic);
}

void GameWindow::unregister_hud_graphic(Graphic* graphic) {
	m_hud_graphics.remove(graphic);
}

void GameWindow::redraw() const {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslated(-round(m_offset_x),-round(m_offset_y),0.0);
	for (list<Graphic*>::const_iterator iter = m_graphics.begin(); iter != m_graphics.end(); ++iter) {
		if (!(*iter)->is_invisible()) {
			(*iter)->draw(this);
		}
	}
	glPopMatrix();
	for (list<Graphic*>::const_iterator iter = m_hud_graphics.begin(); iter != m_hud_graphics.end(); ++iter) {
		if (!(*iter)->is_invisible()) {
			(*iter)->draw(this);
		}
	}
	SDL_GL_SwapBuffers();
}
