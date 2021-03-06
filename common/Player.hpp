/*
 * common/Player.hpp
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

#ifndef LM_COMMON_PLAYER_HPP
#define LM_COMMON_PLAYER_HPP

#include "common/Point.hpp"
#include "common/Packet.hpp"
#include "common/timer.hpp"
#include "PhysicsObject.hpp"
#include <cmath>
#include <string>
#include <string.h>
#include <stdint.h>
#include <vector>

/*
 * A Player represents a player in the game.
 * This class contains functions and variables that are common to both the server and the client.
 * The server should derive a ServerPlayer class that implements server-specific functionality.
 * The client should derive a GraphicalPlayer class that implements client-specific functionality.
 */
class b2Body;
class b2World;
class b2Joint;
class b2Vec2;
 
namespace LM {
	class Player : public PhysicsObject {
	const static float MAX_ANGULAR_VELOCITY = 3.0f;
	
	public:
		enum { MAX_ENERGY = 100 };
		
		const static float PLAYER_WIDTH = 18.0f;
		const static float PLAYER_HEIGHT = 40.0f;

	protected:
		std::string	m_name;		// Specified by the client; should be unique
		uint32_t	m_id;		// Is assigned by the server; should be unique
		char		m_team;		// Should be 'A' or 'B'
		int		m_score;	// How many times has this player shot someone else?
		int		m_energy;	// From 0 to 100, inclusive (where 0 is dead)
		float		m_x;		// The x-coordinate of the upper left point of this player, relative to upper-left of arena
		float		m_y;		// The y-coordinate of the upper left point of this player, relative to upper-left of arena
		float		m_x_vel;	// The x-component of the player's velocity
		float		m_y_vel;	// The y-component of the player's velocity
		float		m_rotation;	// The rotation of the player (Always in degrees)
		float		m_gun_rotation;	// The rotation of the player's gun (Always in degrees)
		float		m_rotational_vel; // How fast the player is spinning (Always in degrees)
		bool		m_is_invisible;	// Is this player inivisible? (should be true while player is waiting to spawn)
		bool		m_is_frozen;	// Is this player frozen? (should be true after the player gets shot)
		uint64_t	m_frozen_at;  // At what time was the player frozen?
		int		m_freeze_time; // How long should the player be frozen for?
		bool		m_is_grabbing_obstacle;	// Is the player grabbing an obstacle?
		long		m_current_weapon_id;	// ID of the current weapon
		b2Body*		m_physics_body; // Box2D physics body for this player
		b2Joint*	m_attach_joint; // Box2D joint that attaches this player to a surface
		b2World*	m_physics; // Box2D physics world pointer
		bool		m_awaiting_detach; // Should we delete the attach joint as soon as possible?
		std::vector<std::pair<b2Vec2, b2Vec2> > m_delayed_force; // Force to apply after the next update
		PhysicsObject*	m_freeze_source; // Holds the most recent object that froze this player.
		uint64_t	m_last_recharge_time; // Holds the most recent time this player was recharged.
		uint64_t	m_last_damage_time; // Holds the most recent time this player was damaged.
	
		virtual void update_location() { }

	public:
		Player(b2World* physics_world = 0);
		Player(const char* name, uint32_t id, char team, float x = 0, float y = 0, float xvel = 0, float yvel = 0, float rotation = 0, b2World* physics_world = 0);
		virtual ~Player();
		
		// Simple getters
		virtual ObjectType get_type() const { return PLAYER; }
		const char* get_name() const { return m_name.c_str(); }
		uint32_t get_id() const { return m_id; }
		char get_team() const { return m_team; }
		int get_score() const { return m_score; }
		int get_energy() const { return m_energy; }
		bool is_alive() const { return get_energy() != 0; }
		bool is_dead() const { return get_energy() == 0; }
		bool is_damaged() const { return get_energy() != MAX_ENERGY; }
		float get_x() const { return m_x; }
		float get_y() const { return m_y; }
		Point get_position() const { return Point(m_x, m_y); }
		float get_x_vel() const { return m_x_vel; }
		float get_y_vel() const { return m_y_vel; }
		Vector get_velocity() const { return Point(m_x_vel, m_y_vel); }
		float get_rotational_vel() const { return m_rotational_vel; }
		float get_rotational_vel_radians() const;
		float get_rotation_degrees() const { return m_rotation; }
		float get_rotation_radians() const;
		float get_gun_rotation_degrees() const { return m_gun_rotation; }
		float get_gun_rotation_radians() const;
		b2Body* get_physics_body() const { return m_physics_body; }
		bool is_invisible() const { return m_is_invisible; }
		bool is_visible() const { return !m_is_invisible; }
		bool is_frozen() const { return m_is_frozen; }
		bool is_unfrozen() const { return !m_is_frozen; }
		bool is_grabbing_obstacle() const { return m_is_grabbing_obstacle; }
		long get_current_weapon_id() const { return m_current_weapon_id; }
		b2Joint* get_attach_joint() const { return m_attach_joint; }
		int get_freeze_time() const { return m_freeze_time; }
		uint64_t get_frozen_at() const { return m_frozen_at; }
		uint64_t get_last_recharge_time() const { return m_last_recharge_time; }
		uint64_t get_last_damage_time() const { return m_last_damage_time; }
		int get_remaining_freeze() const { return std::max<int>(0, m_frozen_at + m_freeze_time - get_ticks()); }
		PhysicsObject* get_freeze_source() const { return m_freeze_source; }
	
		// Return true if this player has the same canonical name as the specified string.
		// Name comparisons are case-insensitive.
		bool compare_name(const char* other_name) const { return strcasecmp(m_name.c_str(), other_name) == 0; }
		
		// Simple setters
		virtual void set_name(const char* name);
		virtual void set_id(uint32_t id);
		virtual void set_team(char team);
		virtual void set_score(int score);
		virtual void add_score(int score_increase);	// Increase the player's score by the given amount
		virtual void set_energy(int energy);
		virtual void change_energy(int energy_increase);	// Increase the player's energy by the given amount
		void set_x(float x);
		void set_y(float y);
		virtual void set_position(float x, float y);
		void set_position(Point p) { set_position(p.x, p.y); }
		void set_velocity(float xvel, float yvel) { set_velocity(Vector(xvel, yvel)); }
		virtual void set_velocity(Vector v);
		virtual void set_rotation_degrees(float rotation);
		void set_rotation_radians(float rotation);
		virtual void set_rotational_vel(float rotation);
		void set_rotational_vel_radians(float rotation);
		virtual void set_gun_rotation_degrees(float gun_rotation);
		void set_gun_rotation_radians(float gun_rotation);
		virtual void set_is_invisible(bool is_invisible);
		virtual void set_is_frozen(bool is_frozen, int64_t freeze_time = 0, PhysicsObject* source = NULL);
		virtual void set_freeze_time(long freeze_time);
		virtual void set_is_grabbing_obstacle(bool);
		virtual void set_current_weapon_id(long id);
		virtual void set_attach_joint(b2Joint* joint);

		void generate_player_update(Packet::PlayerUpdate* p);
		virtual void read_player_update(const Packet::PlayerUpdate& p);
		
		void generate_player_to_server_update(Packet::PlayerToServerUpdate* p);
		virtual void	read_player_to_server_update_packet (PacketReader& packet);
	
		// Initialize the Box2D physics for this player
		virtual void initialize_physics(b2World* world);
		
		// Apply a force to the player
		// UNITS SHOULD BE IN PHYSICS TERMS
		virtual void apply_force(b2Vec2 force_vec);
		virtual void apply_force(b2Vec2 force_vec, b2Vec2 world_point);
		virtual void apply_delayed_force(b2Vec2 force_vec);
		virtual void apply_delayed_force(b2Vec2 force_vec, b2Vec2 world_point);
		
		// Apply a torque to the player
		// UNITS SHOULD BE IN PHYSICS TERMS
		virtual void apply_torque(float torque);
		
		// Update the player's position and rotation to match the physics body
		virtual void update_physics();
		
		// Update the player's position as if the specified amount of time has elapsed
		virtual void update_position(float timescale);
		// Do the same, but with rotation
		virtual void update_rotation(float timescale);
		// Set the player's velocity to 0
		virtual void stop();
		// Update the player's velocity as if it were bouncing off a wall with given angle of incidence (90 == horizontal wall, 180 == vertical wall)
		virtual void bounce(float angle_of_incidence, float velocity_scale);

		bool is_moving() const { return get_velocity() != Vector(0, 0); }
	
		// Reset the player's score to 0
		void reset_score() { set_score(0); }

		// Reset the player's energy to 100
		void reset_energy() { set_energy(MAX_ENERGY); }
	
		struct compare_by_score {
			bool operator() (const Player* a, const Player* b) const {
				return a->get_score() > b->get_score();
			}
		};

		// Read and write PLAYER_UPDATE packets
		void	write_update_packet (PacketWriter& packet) const;
	};
}

#endif
