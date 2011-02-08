/*
 * ai/ReactiveAIController.hpp
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

#ifndef LM_AI_REACTIVEAICONTROLLER_HPP
#define LM_AI_REACTIVEAICONTROLLER_HPP

#include "client/Controller.hpp"
#include <string>
#include "common/physics.hpp"

namespace LM {
	class ReactiveAIController : public Controller, public b2RayCastCallback {
	
	private:
		const static float MAX_AIM_VEL;
		const static unsigned int AIM_TOLERANCE;
		const static float BASE_AIM_UNCERTAINTY;
		const static float QUICK_AIM_CHANGE_ERROR;
		const static unsigned int VISION_RADIUS;
		const static unsigned int GUN_ANGLE_CHANGE_RATE;
		
		enum AimReason {
			JUMP,
			FIRE
		};
	
		int m_changes[2];
		int m_changeset;
		
		float m_wanted_aim; // Radians
		float m_curr_aim; // Radians
		float m_aim_inaccuracy; // Radians
		AimReason m_aim_reason; // Why are we aiming towards the desired location?
		
		void find_desired_aim(const GameLogic& state, uint32_t player_id);
		
		float update_gun(); // Returns the absolute value of the difference between desired and actual angle.
		
		float check_player_visible(const b2World* world, const Player* start_player, const Player* other_player); // Returns the distance to the player, or -1 if not visible.
		float check_gate_visible(const b2World* physics, const Player* start_player, const Gate* gate); // Returns the distance to the gate, or -1 if not visible.
		
		// For ray casts:
		uint32_t		m_ray_hit_player;	// The ID of the closest player hit by the ray, -1 if none or if a wall is in the way.
		float			m_ray_shortest_dist;	// The shortest distance seen in this ray cast.
		Point			m_ray_start;		// The starting point of the ray cast.
		char			m_ray_gate_team;	// The team of the gate that was hit, if any.
		const Gate*		m_enemy_gate;		// The other team's gate.

	public:
		ReactiveAIController();
		
		virtual void update(uint64_t diff, const GameLogic& state, int player_id);

		virtual int get_changes() const;
		virtual float get_aim() const;
		virtual float get_distance() const;
		virtual int get_weapon() const;

		virtual std::wstring get_message() const;
		virtual bool message_is_team_only() const;
		virtual void received_message(const Player* p, const std::wstring& message);
		
		// Box2D Physics Callbacks
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	};
}

#endif
