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
	Mix_Chunk* 	m_freeze_sound;
	Mix_Chunk* 	m_unfreeze_sound;
	Mix_Chunk* 	m_gate_siren_sound;
	Mix_Chunk* 	m_victory_sound;
	Mix_Chunk* 	m_begin_sound;
	Mix_Chunk*	m_defeat_sound;
	Mix_Chunk*	m_click_sound;
	bool		m_sound_on;

public:
	SoundController();
	~SoundController();
	
	void		play_sound(std::string sound);
	int		main();
	bool		is_sound_on() const { return m_sound_on; }
	void		set_sound_on(bool on);
	
};

#endif
