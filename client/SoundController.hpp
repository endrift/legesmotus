/*
 * SoundController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_SOUNDCONTROLLER_HPP
#define LM_CLIENT_SOUNDCONTROLLER_HPP

#include <string>
#include "SDL_mixer.h"

class SoundController {
private:
	Mix_Chunk* 	m_gunshot_sound;

public:
	SoundController();
	~SoundController();
	
	void		play_sound(std::string sound);
	int		main();
	
};

#endif
