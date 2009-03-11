/*
 * GameWindow.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include "GameWindow.hpp"
#include "compat_gl.h"

GameWindow* GameWindow::m_instance = NULL;

GameWindow::GameWindow(int width, int height, int depth, bool fullscreen) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_fullscreen = fullscreen;
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
		//TODO error report of some form
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_ShowCursor(SDL_DISABLE);
	int flags = SDL_HWSURFACE|SDL_OPENGL;
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}
	m_context = SDL_SetVideoMode(width, height, depth, flags);
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

int GameWindow::get_width() const {
	return m_width;
}

int GameWindow::get_height() const {
	return m_height;
}

bool GameWindow::is_fullscreen() const {
	return m_fullscreen;
}

void GameWindow::redraw() const {
	// TODO fill in
	SDL_GL_SwapBuffers();
}
