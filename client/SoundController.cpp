/*
 * SoundController.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "SoundController.hpp"
#include "SDL_mixer.h"
#include "common/PathManager.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;

SoundController::SoundController(PathManager& path_manager) : m_path_manager(path_manager) {
	m_sound_on = true;
	
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		cerr << "Error calling Mix_OpenAudio" << endl;
	}

	m_gunshot_sound = Mix_LoadWAV(m_path_manager.data_path("LMGunshot.ogg", "sounds"));
	if(!m_gunshot_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_unfreeze_sound = Mix_LoadWAV(m_path_manager.data_path("enchant.ogg", "sounds"));
	if(!m_unfreeze_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_freeze_sound = Mix_LoadWAV(m_path_manager.data_path("disenchant.ogg", "sounds"));
	if(!m_freeze_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_gate_siren_sound = Mix_LoadWAV(m_path_manager.data_path("LMGateSiren.ogg", "sounds"));
	if(!m_gate_siren_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_victory_sound = Mix_LoadWAV(m_path_manager.data_path("victory_fanfare.ogg", "sounds"));
	if(!m_victory_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_defeat_sound = Mix_LoadWAV(m_path_manager.data_path("defeatsound.ogg", "sounds"));
	if(!m_victory_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_begin_sound = Mix_LoadWAV(m_path_manager.data_path("clockchime1.ogg", "sounds"));
	if(!m_begin_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
	m_click_sound = Mix_LoadWAV(m_path_manager.data_path("button_click.ogg", "sounds"));
	if(!m_click_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
}

SoundController::~SoundController() {
	Mix_FreeChunk(m_gunshot_sound);
	Mix_FreeChunk(m_freeze_sound);
	Mix_FreeChunk(m_unfreeze_sound);
	Mix_FreeChunk(m_gate_siren_sound);
	Mix_FreeChunk(m_victory_sound);
	Mix_FreeChunk(m_defeat_sound);
	Mix_FreeChunk(m_begin_sound);
	Mix_FreeChunk(m_click_sound);
}

void SoundController::play_sound (string sound) {
	if (!m_sound_on) {
		return;
	}
	
	if(sound == "fire") {
		if(Mix_PlayChannel(-1, m_gunshot_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "freeze") {
		if(Mix_PlayChannel(-1, m_freeze_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "unfreeze") {
		if(Mix_PlayChannel(-1, m_unfreeze_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "gatelower") {
		if(Mix_PlayChannel(-1, m_gate_siren_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "victory") {
		if(Mix_PlayChannel(-1, m_victory_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "begin") {
		if(Mix_PlayChannel(-1, m_begin_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "defeat") {
		if(Mix_PlayChannel(-1, m_defeat_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "click") {
		if(Mix_PlayChannel(-1, m_click_sound, 0) == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	}
}

void SoundController::set_sound_on(bool on) {
	m_sound_on = on;
}
