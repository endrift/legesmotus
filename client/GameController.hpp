/*
 * GameController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMECONTROLLER_HPP
#define LM_CLIENT_GAMECONTROLLER_HPP

#include "GameWindow.hpp"
#include "ClientNetwork.hpp"

class GameController {
private:
	GameWindow* 	m_window;
	ClientNetwork	m_network;
	
	int 		m_screen_width;
	int 		m_screen_height;
	int 		m_pixel_depth;
	bool 		m_fullscreen;
	
public:
	GameController();
	~GameController();
	
	void		run(int lockfps=-1);
};

#endif

