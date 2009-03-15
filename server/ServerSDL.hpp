/*
 * server/ServerSDL.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_SERVER_SERVERSDL_HPP
#define LM_SERVER_SERVERSDL_HPP

#include "common/CommonSDL.hpp"

class ServerSDL {
private:
	CommonSDL	common_sdl;

public:
	ServerSDL();
	~ServerSDL();

};

#endif
