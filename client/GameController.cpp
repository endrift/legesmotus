/*
 * GameController.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "ClientNetwork.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

GameController::GameController() {
	init(1024, 768, 24, true);
}

GameController::GameController(int width, int height) {
	init(width, height, 24, true);
}

GameController::~GameController() {
	m_window->destroy_instance();
}

void GameController::init(int width, int height, int depth, bool fullscreen) {
	initialize_key_bindings();
	
	m_screen_width = width;
	m_screen_height = height;
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);
}

void GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = SDL_GetTicks();
	unsigned long lastmoveframe = SDL_GetTicks();
	
	/* 1 second / FPS = milliseconds per frame */
	unsigned long delay = 1000 / lockfps;
	
	while(m_quit_game == false) {
		process_input();
		
		if (m_quit_game == true) {
			break;
		}
		
		move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
		
		lastmoveframe = SDL_GetTicks();
		
		// Update graphics if frame rate is correct
		unsigned long currframe = SDL_GetTicks();
		if((currframe - startframe) >= delay) {
			// the framerate:
			int framerate = (1000/(currframe - startframe));
			
			m_window->redraw();
			startframe = SDL_GetTicks();
		}
	}
}

void GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}

void GameController::process_input() {
	SDL_Event event;
	
	while(SDL_PollEvent(&event) != 0) {
		switch(event.type) {
			case SDL_KEYDOWN:
				//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
				break;

			case SDL_KEYUP:
				break;
				
			case SDL_MOUSEMOTION:
				// Send motion to the GameWindow, telling it where the mouse is.
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				break;
				
			case SDL_MOUSEBUTTONUP:
				break;
				
			default:
				break;
		}
	}
	
	parse_key_input();
}

void GameController::initialize_key_bindings() {
	// -1 = unused
	m_key_bindings.quit = SDLK_ESCAPE;
	m_key_bindings.jump = -1;
	m_key_bindings.show_overlay = -1;
	m_key_bindings.show_menu = -1;
	m_key_bindings.open_chat = -1;
}

void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
	if (m_keys[m_key_bindings.quit]) {
		cerr << "Quit key pressed - quitting." << endl;
		m_quit_game = true;
	} else if (m_keys[m_key_bindings.jump]) {
		// TODO: Cause character to jump.
	} else if (m_keys[m_key_bindings.show_overlay]) {
		// TODO: Show the overlay.
	} else if (m_keys[m_key_bindings.open_chat]) {
		// TODO: Show the chat interface.
	} else if (m_keys[m_key_bindings.show_menu]) {
		// TODO: Show the menu.
	}
}

void GameController::move_objects(float timescale) {
}
