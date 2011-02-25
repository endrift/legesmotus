/*
 * ai/AI.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_AI_AI_HPP
#define LM_AI_AI_HPP

#include <string>
#include "common/physics.hpp"
#include <list>

namespace LM {
	class GameLogic;
	class Player;
	class StateTranslator;
	class PhysicsObject;
	class Gate;

	class AI {
	private:
		struct RayCastResult {
			PhysicsObject*	start_object;	// The object (if any) where this ray started
			b2Vec2		ray_start;	// The starting point of the ray cast
			float		ray_direction;	// The angle (in radians) at which the ray was cast
			b2Vec2		ray_end;	// The maximum point on the ray
			PhysicsObject* 	closest_object; // The closest object
			float		shortest_dist;	// The closest hit-point on that object
			b2Vec2		hit_point;	// The point where the ray hit
		};
	
		GameLogic* m_logic;
		Player* m_player;
		Player* m_other_player;
		
		std::list<std::pair<const char*, float> > m_varlist;
		
		RayCastResult m_ray_cast;
		
	public:
		AI(GameLogic* logic);
		~AI();
		
		void set_own_player(Player* player);
		Player* get_own_player();
		
		void set_other_player(Player* other_player);
		Player* get_other_player();
		
		const std::list<std::pair<const char*, float> >& get_vars();
		
		float get_fuzzy_input_value(StateTranslator* translator, const std::string& subsection);
		
		// Begin methods used to get various data that can be passed to the Fuzzy Logic system.
		
		// Distance between players, in game (not physics) units
		float dist_between_players(Player* first, Player* second) const;
		
		// Distance to the gate, in game (not physics) units
		float dist_to_own_gate(Player* player) const;
		float dist_to_enemy_gate(Player* player) const;
		
		// Gate progress, from 0 to 1, if the enemy is holding the gate, 0 otherwise.
		float holding_gate(Player* player) const;
		
		// Percent (0-1) of energy the enemy player has
		float energy_percent(Player* player) const;
		
		// Amount of cooldown (milliseconds) remaining on the player's gun. NOTE: Only valid for your own player
		float gun_cooldown(Player* player) const;
		
		// Magnitude of angle difference between current gun rotation and rotation to hit enemy
		float gun_angle_to_player(Player* player, Player* other) const;
		
		// Distance to nearest object that could be hit by the player divided by the player's velocity
		// Note: if the object that could be hit is moving, this will not check whether it will actually hit
		float time_to_impact(Player* player);
		
		// Distance to the player/gate (game units, not physics), if it can be seen, or max float value if not.
		float can_see_player(Player* player, Player* other_player, float max_radius = -1);
		float can_see_gate(Player* player, Gate* gate, float max_radius = -1);
		
		// End data-retrieval methods.
		
		// Boolean methods that could be necessary, but cannot be converted into floating point values:
		bool is_active(Player* player) const;
		bool grabbing_wall(Player* player) const;
	};
}

#endif
