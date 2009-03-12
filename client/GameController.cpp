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
	m_screen_width = 1024;
	m_screen_height = 768;
	m_pixel_depth = 24;
	m_fullscreen = true;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);
}

GameController::GameController(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
	m_pixel_depth = 24;
	m_fullscreen = true;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);
}

GameController::~GameController() {
	m_window->destroy_instance();
}

void	GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	SDL_Delay(1000);
	
	int i = 0;
	while(i < 5) {
		m_window->redraw();
		SDL_Delay(1000);
		i++;
	}
}

void	GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}
