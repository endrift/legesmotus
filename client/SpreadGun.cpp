/*
 * client/SpreadGun.cpp
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

#include "SpreadGun.hpp"
#include "GameController.hpp"
#include "BaseMapObject.hpp"
#include "common/Player.hpp"
#include "common/StringTokenizer.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"
#include <map>
#include <list>


using namespace LM;
using namespace std;

SpreadGun::SpreadGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_damage = 30;
	m_damage_degradation = 0.01;
	m_nbr_projectiles = 5;
	m_angle = to_radians(10);
	m_cooldown = 700;
	m_recoil = 1.5;

	m_impact_graphic = "sshot.png";

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}
}

bool	SpreadGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "damage=", 7) == 0) {
		m_damage = atoi(param_string + 7);
	} else if (strncmp(param_string, "degradation=", 12) == 0) {
		m_damage_degradation = atof(param_string + 12);
	} else if (strncmp(param_string, "projectiles=", 12) == 0) {
		m_nbr_projectiles = atoi(param_string + 12);
	} else if (strncmp(param_string, "angle=", 6) == 0) {
		m_angle = to_radians(atof(param_string + 6));
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void	SpreadGun::fire(Player& player, GameController& gc, Point startpos, double initial_direction) {
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

	//
	// Figure out who/what we hit
	//
	std::list<Point>		hit_points;
	std::map<Player*, int>		hit_players;
	std::map<Player*, double>	hit_player_damage;

	double				direction = initial_direction - m_angle / 2;
	const double			angle_increment = m_angle / (m_nbr_projectiles - 1.0);
	for (int i = 0; i < m_nbr_projectiles; ++i) {
		// Find the nearest object that this hit
		BaseMapObject*	hit_map_object = NULL;
		Player*		hit_player = NULL;
		Point		hit_point(gc.find_shootable_object(startpos, direction, hit_map_object, hit_player));

		// If this shot hit a map object, tell the map object about it
		if (hit_map_object != NULL) {
			if (!hit_map_object->shot(gc, player, hit_point, direction)) {
				// Instead of absorbing the shot, the map object redirected it
				hit_point = Point(-1, -1);
			}
		}
		if (hit_point.x != -1 && hit_point.y != -1) {
			hit_points.push_back(hit_point);
		}
		
		if (hit_player != NULL) {
			// A player was hit
			++hit_players[hit_player];

			double		damage = m_damage - m_damage_degradation * Point::distance(startpos, hit_point);
			if (damage > 0) {
				hit_player_damage[hit_player] += damage;
			}
			//std::cerr << "Hit with damage " << damage << '\n';
		}

		direction += angle_increment;
	}

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
	for (std::map<Player*, int>::const_iterator it(hit_players.begin()); it != hit_players.end(); ++it) {
		Player*	hit_player = it->first;
		int	times_hit = it->second;
		double	damage = hit_player_damage[hit_player];

		if (!hit_player->is_frozen() && !hit_player->is_dead()) {
			gc.play_sound("hit");
		}

		PacketWriter	hit_packet(PLAYER_HIT_PACKET);
		hit_packet << player.get_id() << get_id() << hit_player->get_id() << times_hit << damage << initial_direction - M_PI;
		gc.send_reliable_packet(hit_packet);
	}
}

void	SpreadGun::discharged(Player& player, GameController& gc, StringTokenizer& data) {
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

void	SpreadGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) {
	int	times_hit;
	double	damage;
	double	angle;
	data >> times_hit >> damage >> angle;

	// Adjust the shot player's velocity
	shot_player.set_velocity(shot_player.get_velocity() - Vector::make_from_magnitude((m_recoil / m_nbr_projectiles) * times_hit, angle));

	//std::cerr << "I was hit " << times_hit << " times at angle " << angle << " with a total damage of " << damage << '\n';

	if (has_effect) {
		// Deal damage to the player
		gc.damage(damage, &shooting_player);
	}
}

/*
void	SpreadGun::select(Player& selecting_player, GameController& gc) {
}
*/

void	SpreadGun::reset() {
	m_last_fired_time = 0;
}

uint64_t	SpreadGun::get_remaining_cooldown() const
{
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

