/*
 * common/CommonSDL.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "CommonSDL.hpp"
#include "LMException.hpp"
#include "compat_sdl.h"
#include "SDL_net.h"

CommonSDL::CommonSDL() {
	if (SDL_Init(0) == - 1) {
		throw LMException("Failed to initialize SDL.");
	}
	if (SDLNet_Init() == -1) {
		SDL_Quit();
		throw LMException("Failed to initialize SDLNet.");
	}
}

CommonSDL::~CommonSDL() {
	SDLNet_Quit();
	SDL_Quit();
}
