/*
 * SoundController.hpp
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

#ifndef LM_CLIENT_SOUNDCONTROLLER_HPP
#define LM_CLIENT_SOUNDCONTROLLER_HPP

#include <string>
#include "SDL_mixer.h"

class PathManager;

class SoundController {
private:
	PathManager&	m_path_manager;
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
	explicit SoundController(PathManager&);
	~SoundController();
	
	void		play_sound(std::string sound);
	bool		is_sound_on() const { return m_sound_on; }
	void		set_sound_on(bool on);
	
};

#endif
