/*
 * client/ThawGun.cpp
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

#include "ThawGun.hpp"
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

ThawGun::ThawGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_freeze_reduction = 500;
	m_energy_cost = 0;
	m_cooldown = 100;
	m_recoil = 0;
	m_is_continuous = true;
	m_total_ammo = 10;
	m_ammo_recharge = 600;

	m_impact_graphic = "sshot.png";

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}

	reset();
}

bool	ThawGun::parse_param(const char* param_string) {
	if (strncmp(param_string, "reduction=", 10) == 0) {
		m_freeze_reduction = atol(param_string + 10);
	} else if (strncmp(param_string, "cost=", 5) == 0) {
		m_energy_cost = atoi(param_string + 5);
	} else if (strncmp(param_string, "cooldown=", 9) == 0) {
		m_cooldown = atol(param_string + 9);
	} else if (strncmp(param_string, "recoil=", 7) == 0) {
		m_recoil = atof(param_string + 7);
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
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void	ThawGun::fire(Player& player, GameController& gc, Point startpos, double direction) {
	if (m_last_fired_time != 0 && get_ticks() - m_last_fired_time < m_cooldown) {
		// Firing too soon
		return;
	}

	if (player.get_energy() < m_energy_cost) {
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

	gc.damage(m_energy_cost, &player);

	// Cause recoil if the player is not hanging onto a wall.
	if (!player.is_grabbing_obstacle()) {
		player.set_velocity(player.get_velocity() - Vector::make_from_magnitude(m_recoil, direction));
	}

	gc.play_sound("fire");

	// Find the nearest object that this hit
	BaseMapObject*		hit_map_object = NULL;
	Player*			hit_player = NULL;
	Point			hit_point(gc.find_shootable_object(startpos, direction, hit_map_object, hit_player));

	// If this shot hit a map object, tell the map object about it
	if (hit_map_object != NULL) {
		if (!hit_map_object->shot(gc, player, hit_point, direction)) {
			// Instead of absorbing the shot, the map object redirected it
			hit_point = Point(-1, -1);
		}
	}
	
	// Flash the muzzle
	gc.show_muzzle_flash();

	// Create the gun fired packet and send it, and display the shot hit point.
	PacketWriter		fired_packet(WEAPON_DISCHARGED_PACKET);
	fired_packet << player.get_id() << get_id() << startpos << direction;
	if (hit_point.x != -1 && hit_point.y != -1) {
		fired_packet << hit_point;
		gc.show_bullet_impact(hit_point, m_impact_graphic.c_str());
	}
	gc.send_packet(fired_packet);
	
	// If a player was hit, send a packet about it
	if (hit_player != NULL) {
		if (!hit_player->is_frozen() && !hit_player->is_dead()) {
			gc.play_sound("hit");
		}

		PacketWriter	hit_packet(PLAYER_HIT_PACKET);
		hit_packet << player.get_id() << get_id() << hit_player->get_id() << direction - M_PI;
		gc.send_reliable_packet(hit_packet);
	}
}

void	ThawGun::discharged(Player& player, GameController& gc, StringTokenizer& data) {
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

void	ThawGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) {
	double	angle;
	data >> angle;
	
	// Adjust the shot player's velocity
	shot_player.set_velocity(shot_player.get_velocity() - Vector::make_from_magnitude(m_recoil, angle));

	gc.reduce_freeze_time(m_freeze_reduction);
}

void	ThawGun::reset() {
	m_last_fired_time = 0;
	m_current_ammo = m_total_ammo;
}

uint64_t	ThawGun::get_remaining_cooldown() const
{
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

int	ThawGun::get_current_ammo () const
{
	if (m_last_fired_time && m_ammo_recharge) {
		// Take into account the ammo recharge that has occurred since last firing
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		return min<int>(m_total_ammo, m_current_ammo + time_since_fire / m_ammo_recharge);
	}
	return m_current_ammo;
}

