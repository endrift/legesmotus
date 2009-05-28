/*
 * common/Player.hpp
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

#ifndef LM_COMMON_PLAYER_HPP
#define LM_COMMON_PLAYER_HPP

#include <cmath>
#include <string>
#include <string.h>
#include <stdint.h>

/*
 * A Player represents a player in the game.
 * This class contains functions and variables that are common to both the server and the client.
 * The server should derive a ServerPlayer class that implements server-specific functionality.
 * The client should derive a GraphicalPlayer class that implements client-specific functionality.
 */
class Player {
protected:
	std::string	m_name;		// Specified by the client; should be unique
	uint32_t	m_id;		// Is assigned by the server; should be unique
	char		m_team;		// Should be 'A' or 'B'
	int		m_score;	// How many times has this player shot someone else?
	double		m_x;		// The x-coordinate of the upper left point of this player, relative to upper-left of arena
	double		m_y;		// The y-coordinate of the upper left point of this player, relative to upper-left of arena
	double		m_x_vel;	// The x-component of the player's velocity
	double		m_y_vel;	// The y-component of the player's velocity
	double		m_rotation;	// The rotation of the player (Always in degrees)
	double		m_gun_rotation;	// The rotation of the player's gun (Always in degrees)
	double		m_rotational_vel; // How fast the player is spinning (Always in degrees)
	bool		m_is_invisible;	// Is this player inivisible? (should be true while player is waiting to spawn)
	bool		m_is_frozen;	// Is this player frozen? (should be true after the player gets shot)

public:
	Player();
	Player(const char* name, uint32_t id, char team, double x = 0, double y = 0, double xvel = 0, double yvel = 0, double rotation = 0);
	virtual ~Player();
	
	// Simple getters
	const char* get_name() const { return m_name.c_str(); }
	uint32_t get_id() const { return m_id; }
	char get_team() const { return m_team; }
	int get_score() const { return m_score; }
	double get_x() const { return m_x; }
	double get_y() const { return m_y; }
	double get_x_vel() const { return m_x_vel; }
	double get_y_vel() const { return m_y_vel; }
	double get_rotational_vel() const { return m_rotational_vel; }
	double get_rotational_vel_radians() const;
	double get_rotation_degrees() const { return m_rotation; }
	double get_rotation_radians() const;
	double get_gun_rotation_degrees() const { return m_gun_rotation; }
	double get_gun_rotation_radians() const;
	bool is_invisible() const { return m_is_invisible; }
	bool is_visible() const { return !m_is_invisible; }
	bool is_frozen() const { return m_is_frozen; }
	bool is_unfrozen() const { return !m_is_frozen; }

	// Return true if this player has the same canonical name as the specified string.
	// Name comparisons are case-insensitive.
	bool compare_name(const char* other_name) const { return strcasecmp(m_name.c_str(), other_name) == 0; }
	
	// Simple setters
	void set_name(const char* name);
	virtual void set_id(uint32_t id);
	virtual void set_team(char team);
	virtual void set_score(int score);
	virtual void add_score(int score_increase);	// Increase the player's score by the given amount
	virtual void set_x(double x);
	virtual void set_y(double y);
	virtual void set_position(double x, double y);
	virtual void set_x_vel(double xvel);
	virtual void set_y_vel(double yvel);
	virtual void set_velocity(double xvel, double yvel);
	virtual void set_rotation_degrees(double rotation);
	virtual void set_rotation_radians(double rotation);
	virtual void set_rotational_vel(double rotation);
	virtual void set_rotational_vel_radians(double rotation);
	virtual void set_gun_rotation_degrees(double gun_rotation);
	virtual void set_gun_rotation_radians(double gun_rotation);
	virtual void set_is_invisible(bool is_invisible);
	virtual void set_is_frozen(bool is_frozen);

	// Update the player's position as if the specified number of units time have elapsed
	virtual void update_position(unsigned long timediff);

	// Reset the player's score to 0
	void reset_score() { set_score(0); }

	struct compare_by_score {
		bool operator() (const Player* a, const Player* b) const {
			return a->get_score() > b->get_score();
		}
	};
};

#endif
