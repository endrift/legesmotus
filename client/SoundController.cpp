/*
 * SoundController.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "SoundController.hpp"
#include "SDL_mixer.h"
#include <iostream>

using namespace std;

SoundController::SoundController() {
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		cout << "Error calling Mix_OpenAudio" << endl;
	}

	m_gunshot_sound = Mix_LoadWAV("data/sounds/LMGunshot.ogg");
	if(!m_gunshot_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_freeze_sound = Mix_LoadWAV("data/sounds/enchant.ogg");
	if(!m_freeze_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_unfreeze_sound = Mix_LoadWAV("data/sounds/disenchant.ogg");
	if(!m_unfreeze_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_gate_siren_sound = Mix_LoadWAV("data/sounds/LMGateSiren.ogg");
	if(!m_gate_siren_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_victory_sound = Mix_LoadWAV("data/sounds/LMGateSiren.ogg");
	if(!m_victory_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_begin_sound = Mix_LoadWAV("data/sounds/clockchime1.ogg");
	if(!m_begin_sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}
}

SoundController::~SoundController() {
	delete m_gunshot_sound;
}

void SoundController::play_sound (string sound) {
	if(sound == "fire") {
		if(Mix_PlayChannel(-1, m_gunshot_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "freeze") {
		if(Mix_PlayChannel(-1, m_freeze_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "unfreeze") {
		if(Mix_PlayChannel(-1, m_unfreeze_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "gatelower") {
		if(Mix_PlayChannel(-1, m_gate_siren_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "victory") {
		if(Mix_PlayChannel(-1, m_victory_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	}
	else if(sound == "begin") {
		if(Mix_PlayChannel(-1, m_begin_sound, 0) == -1) {
			printf("Mix_PlayChannel: %s\n", Mix_GetError());
		}
	}
}

