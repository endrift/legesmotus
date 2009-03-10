/*
 * GameController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMECONTROLLER_HPP
#define LM_CLIENT_GAMECONTROLLER_HPP

class GameWindow;
class ClientNetwork;

class GameController {
private:
	GameWindow 	m_window;
	ClientNetwork	m_network;
	
public:
	GameController();

	void		run(int lockfps=-1);
};

#endif

