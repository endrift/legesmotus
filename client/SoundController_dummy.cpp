/*
 * SoundController_dummy.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "SoundController.hpp"

using namespace std;

SoundController* SoundController::m_instance = NULL;

SoundController::SoundController(GameController& parent, PathManager& path_manager) : m_path_manager(path_manager), m_parent(parent) {
	m_sound_on = false;
}

SoundController::~SoundController() {
	// Nothing to do
}

SoundController* SoundController::get_instance(GameController& parent, PathManager& path_manager) {
	if (m_instance == NULL) {
		m_instance = new SoundController(parent, path_manager);
	}
	return m_instance;
}

SoundController* SoundController::get_instance() {
	return m_instance;
}

void SoundController::destroy_instance() {
	delete m_instance;
	m_instance = NULL;
}

int SoundController::play_sound (string sound) {
	return -1;
}

void SoundController::halt_sound(int channel) {
	// Nothing to do
}

void SoundController::set_sound_on(bool on) {
	// Nothing to do
}

void SoundController::channel_done(int channel) {
	// Nothing to do
}

void SoundController::channel_finished(int channel) {
	// Nothing to do
}
