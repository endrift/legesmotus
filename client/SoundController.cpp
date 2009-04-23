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

	m_gunshot_sound = Mix_LoadWAV("data/sounds/LMGunshot.wav");
	if(!m_gunshot_sound) {
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
	}
}

