/*
 * client/AreaGun.cpp
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

#include "AreaGun.hpp"
#include "GameController.hpp"
#include "BaseMapObject.hpp"
#include "common/Player.hpp"
#include "common/StringTokenizer.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"
#include <map>
#include <list>
#include <iostream>


using namespace LM;
using namespace std;

AreaGun::AreaGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_damage = 30;
	m_damage_degradation = 0.01;
	m_cooldown = 700;
	m_recoil = 1.5;
	
	m_area = new Polygon();
	m_pivot = Point();

	m_impact_graphic = "sshot.png";

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}
}

AreaGun::~AreaGun() {
	delete m_area;
}

bool	AreaGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "damage=", 7) == 0) {
		m_damage = atoi(param_string + 7);
	} else if (strncmp(param_string, "degradation=", 12) == 0) {
		m_damage_degradation = atof(param_string + 12);
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
	} else if (strncmp(param_string, "pivot=", 6) == 0) {
		m_pivot = Point::make_from_string(param_string + 6);
	} else if (strncmp(param_string, "freeze=", 7) == 0) {
		m_freeze_time = atol(param_string + 7);
	} else if (strncmp(param_string, "force=", 6) == 0) {
		m_force = atof(param_string + 6);
	} else if (strncmp(param_string, "shape=", 6) == 0) {
		m_area = Polygon::make_from_string(param_string + 6, Point(0, 0));
	/*} else if (strncmp(param_string, "move=", 5) == 0) {
		if (strcmp(param_string + 5, "true") == 0) {
			m_move_shot = true;
		} else {
			m_move_shot = false;
		}*/
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void	AreaGun::fire(Player& player, GameController& gc, Point startpos, double initial_direction) {
	if (m_last_fired_time != 0 && get_ticks() - m_last_fired_time < m_cooldown) {
		// Firing too soon
		return;
	}
	
	// Cause recoil if the player is not hanging onto a wall.
	if (!player.is_grabbing_obstacle()) {
		player.set_velocity(player.get_velocity() - Vector::make_from_magnitude(m_recoil, initial_direction));
	}

	m_last_fired_time = get_ticks();
	gc.play_sound("fire");
	gc.show_muzzle_flash();
	
	// Rotate and translate the polygon
	m_area->rotate(to_degrees(initial_direction));
	m_area->translate(player.get_x() - m_pivot.x, player.get_y() - m_pivot.y);
	m_pivot += player.get_position();

	//
	// Figure out who/what we hit
	//
	std::list<Point>		hit_points;
	std::list<Player*>		hit_players;
	std::map<Player*, double>	hit_player_damage;

	// Find players inside the area
	hit_points = gc.shoot_in_region(*m_area, m_pivot, hit_players);

	//
	// Send a weapon discharged packet so other players know we fired
	//
	PacketWriter	discharged_packet(WEAPON_DISCHARGED_PACKET);
	discharged_packet << player.get_id() << get_id() << startpos << initial_direction;
	while (!hit_points.empty()) {
		gc.show_bullet_impact(hit_points.front(), m_impact_graphic.c_str()); // Also show the bullet impact for each hit point
		discharged_packet << hit_points.front();
		hit_points.pop_front();
	}
	gc.send_packet(discharged_packet);

	//
	// Iterate through all the hit players and send packets about each one
	//
	for (std::list<Player*>::const_iterator it(hit_players.begin()); it != hit_players.end(); ++it) {
		Player*	hit_player = *it;
		double	damage = hit_player_damage[hit_player];

		if (!hit_player->is_frozen() && !hit_player->is_dead()) {
			gc.play_sound("hit");
		}
		
		Point playerpos = hit_player->get_position();
		double x_dist = playerpos.x - m_pivot.x;
		double y_dist = playerpos.y - m_pivot.y;
		double direction = atan2(y_dist, x_dist);

		PacketWriter	hit_packet(PLAYER_HIT_PACKET);
		hit_packet << player.get_id() << get_id() << hit_player->get_id() << damage << direction - M_PI;
		gc.send_reliable_packet(hit_packet);
	}
	
	// Return the polygon to its original position/rotation
	m_pivot -= player.get_position();
	m_area->translate(m_pivot.x - player.get_x(), m_pivot.y - player.get_y());
	m_area->rotate(-to_degrees(initial_direction));
}

void	AreaGun::discharged(Player& player, GameController& gc, StringTokenizer& data) {
	Point	startpos;
	double	direction;
	data >> startpos >> direction;

	// Play a sound
	gc.play_sound("fire");

	// Activate the radar blip
	gc.activate_radar_blip(player);

	// Display a bullet impact point for every place a projectile landed
	while (data.has_more()) {
		Point	endpos;
		data >> endpos;

		gc.show_bullet_impact(endpos, m_impact_graphic.c_str());
	}
}

void	AreaGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) {
	double	damage;
	double	angle;
	data >> damage >> angle;

	if (has_effect) {
		// Deal damage to the player
		if (!gc.damage(m_damage, &shooting_player)) {
			// Player was not killed, so freeze the player based on this weapon's effects
			gc.freeze(m_freeze_time);
		}

		// And change his velocity
		shot_player.set_velocity(shot_player.get_velocity() - Vector::make_from_magnitude(m_force, angle));
	}
}

/*
void	AreaGun::select(Player& selecting_player, GameController& gc) {
}
*/

void	AreaGun::reset() {
	m_last_fired_time = 0;
}

uint64_t	AreaGun::get_remaining_cooldown() const
{
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

