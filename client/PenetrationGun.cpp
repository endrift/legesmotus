/*
 * client/PenetrationGun.cpp
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

#include "PenetrationGun.hpp"
#include "GameController.hpp"
#include "BaseMapObject.hpp"
#include "common/Player.hpp"
#include "common/StringTokenizer.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"
#include <algorithm>
#include <stdlib.h>

using namespace LM;
using namespace std;

PenetrationGun::PenetrationGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_freeze_time = 0;
	m_damage = 100;
	m_cooldown = 700;
	m_recoil = 1.5;
	m_inaccuracy = 0;
	m_is_continuous = false;
	m_total_ammo = 0;
	m_ammo_recharge = 500;
	m_max_penetration = 2;
	m_penetrates_players = 0.8;
	m_penetrates_walls = 0;

	m_impact_graphic = "shot.png";

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}

	reset();
}

bool	PenetrationGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "freeze=", 7) == 0) {
		m_freeze_time = atol(param_string + 7);
	} else if (strncmp(param_string, "damage=", 7) == 0) {
		m_damage = atoi(param_string + 7);
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
	} else if (strncmp(param_string, "inaccuracy=", 11) == 0) {
		m_inaccuracy = to_radians(atof(param_string + 11));
	} else if (strcmp(param_string, "ammo=unlimited") == 0) {
		m_total_ammo = 0;
	} else if (strncmp(param_string, "ammo=", 5) == 0) {
		m_total_ammo = atoi(param_string + 5);
	} else if (strncmp(param_string, "ammo_recharge=", 14) == 0) {
		m_ammo_recharge = atol(param_string + 14);
	} else if (strcmp(param_string, "continuous") == 0) {
		m_is_continuous = true;
	} else if (strcmp(param_string, "notcontinuous") == 0) {
		m_is_continuous = false;
	} else if (strncmp(param_string, "penetrate_players=", 18) == 0) {
		m_penetrates_players = atof(param_string + 18);
	} else if (strncmp(param_string, "penetrate_walls=", 16) == 0) {
		m_penetrates_walls = atof(param_string + 16);
	} else if (strncmp(param_string, "max_penetration=", 16) == 0) {
		m_max_penetration = atof(param_string + 16);
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void	PenetrationGun::fire(Player& player, GameController& gc, Point startpos, double direction) {
	if (m_last_fired_time != 0 && get_ticks() - m_last_fired_time < m_cooldown) {
		// Firing too soon
		return;
	}

	// Recharge ammo, if applicable
	if (m_total_ammo) {
		m_current_ammo = get_current_ammo();
	}

	m_last_fired_time = get_ticks(); // Record this *before* checking for ammo, so that the player has to ease off the trigger before ammo starts recharging

	if (m_total_ammo) {
		if (m_current_ammo == 0) {
			// Out of ammo
			return;
		}
		--m_current_ammo;
	}

	// Randomly change the direction anywhere within the + or - m_inaccuracy angle.
	direction += (rand()/(RAND_MAX+1.0)) * (m_inaccuracy*2) - m_inaccuracy;

	// Cause recoil if the player is not hanging onto a wall.
	if (!player.is_grabbing_obstacle()) {
		player.set_velocity(player.get_velocity() - Vector::make_from_magnitude(m_recoil, direction));
	}

	gc.play_sound("fire");

	// Find the objects hit by this shot in order
	std::list<Point>		hit_points;
	std::map<Player*, double>	hit_players;
	multiset<GameController::HitObject>		hit_objects;
	
	gc.shoot_in_line(startpos, direction, hit_objects);
	
	double				current_damage = m_damage;
	int				objects_penetrated = 0;
	
	if (!hit_objects.empty()) {
		multiset<GameController::HitObject>::iterator it;

  		for ( it=hit_objects.begin() ; it != hit_objects.end(); it++ ) {
  			if (objects_penetrated >= m_max_penetration or current_damage <= 0) {
  				break;
  			}
  		
  			GameController::HitObject obj = *it;
  			if (obj.map_object != NULL) {
  				BaseMapObject* hit_map_object = obj.map_object;
  				// If this shot hit a map object, tell the map object about it
				if (hit_map_object != NULL) {
					if (!hit_map_object->shot(gc, player, obj.point, direction)) {
						// Instead of absorbing the shot, the map object redirected it
						// TODO: What do we do here?
					} else {
						hit_points.push_back(obj.point);
						current_damage = current_damage * m_penetrates_walls;
						objects_penetrated++;
					}
				}
  			} else if (obj.player != NULL) {
				Player* hit_player = obj.player;
				hit_points.push_back(obj.point);
				
				hit_players[hit_player] += current_damage;
				
				current_damage = current_damage * m_penetrates_players;
				objects_penetrated++;
			} else {
				hit_points.push_back(obj.point);
				current_damage = current_damage * m_penetrates_walls;
				objects_penetrated++;
			}
  		}
	}
	
	// Flash the muzzle
	gc.show_muzzle_flash();

	// Create the gun fired packet and send it, and display the shot hit point.
	PacketWriter		fired_packet(WEAPON_DISCHARGED_PACKET);
	fired_packet << player.get_id() << get_id() << startpos << direction;
	while (!hit_points.empty()) {
		gc.show_bullet_impact(hit_points.front(), m_impact_graphic.c_str()); // Also show the bullet impact for each hit point
		fired_packet << hit_points.front();
		hit_points.pop_front();
	}
	gc.send_packet(fired_packet);
	
	//
	// Iterate through all the hit players and send packets about each one
	//
	for (std::map<Player*, double>::const_iterator it(hit_players.begin()); it != hit_players.end(); ++it) {
		Player*	hit_player = it->first;
		double	damage = it->second;

		if (!hit_player->is_frozen() && !hit_player->is_dead()) {
			gc.play_sound("hit");
		}

		PacketWriter	hit_packet(PLAYER_HIT_PACKET);
		hit_packet << player.get_id() << get_id() << hit_player->get_id() << damage << direction - M_PI;
		gc.send_reliable_packet(hit_packet);
	}
}

void	PenetrationGun::discharged(Player& player, GameController& gc, StringTokenizer& data) {
	Point	startpos;
	double	rotation;
	data >> startpos >> rotation;

	// Play a sound
	gc.play_sound("fire");

	// Activate the radar blip
	gc.activate_radar_blip(player);

	if (data.has_more()) {
		Point	endpos;
		data >> endpos;

		gc.show_bullet_impact(endpos, m_impact_graphic.c_str());
	}
}

void	PenetrationGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) {
	double	angle;
	double 	damage;
	data >> damage >> angle;

	// Adjust the shot player's velocity
	shot_player.set_velocity(shot_player.get_velocity() - Vector::make_from_magnitude(m_recoil, angle));

	if (has_effect) {
		// Deal damage to the player
		if (!gc.damage(damage, &shooting_player)) {
			// Player was not killed, so freeze the player based on this weapon's effects
			gc.freeze(m_freeze_time);
		}
	}
}

/*
void	PenetrationGun::select(Player& selecting_player, GameController& gc) {
}
*/

void	PenetrationGun::reset() {
	m_last_fired_time = 0;
	m_current_ammo = m_total_ammo;
}

uint64_t	PenetrationGun::get_remaining_cooldown() const
{
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

int	PenetrationGun::get_current_ammo () const
{
	if (m_last_fired_time && m_ammo_recharge) {
		// Take into account the ammo recharge that has occurred since last firing
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		return min<int>(m_total_ammo, m_current_ammo + time_since_fire / m_ammo_recharge);
	}
	return m_current_ammo;
}

