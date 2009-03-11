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
	m_screen_width = 1680;
	m_screen_height = 1050;
	m_pixel_depth = 32;
	m_fullscreen = true;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);
}

GameController::~GameController() {
	m_window->destroy_instance();
}

void	GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
}
