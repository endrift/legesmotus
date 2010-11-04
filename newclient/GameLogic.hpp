/*
 * newclient/GameLogic.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_NEWCLIENT_GAMELOGIC_HPP
#define LM_NEWCLIENT_GAMELOGIC_HPP

#include "common/Player.hpp"
#include <map>

class b2World;

namespace LM {
	class Map;

	class GameLogic {
	const static float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const static int VEL_ITERATIONS = 10;
	const static int POS_ITERATIONS = 10;
	
	private:
		std::map<uint32_t, Player*> m_players;
		Map* m_map;
		b2World* m_physics;

	public:
		GameLogic(Map* map);
		~GameLogic();

		void add_player(Player* player);
		void remove_player(uint32_t id);
		
		void update_map(Map* map);
		
		// Run the next step of the game logic.
		void step();

		Player* get_player(uint32_t id);
	};
}

#endif
