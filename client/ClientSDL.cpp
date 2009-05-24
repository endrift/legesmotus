/*
 * client/ClientSDL.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "ClientSDL.hpp"
#include "common/LMException.hpp"
#include "SDL.h"

// See ClientSDL.hpp for a description of this class's purpose.

ClientSDL::ClientSDL() {
	if (SDL_Init(0) == -1) {
		throw LMException("Failed to initialize SDL.");
	}
}

ClientSDL::~ClientSDL() {
	SDL_Quit();
}

