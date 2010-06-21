/*
 * client/ChargeGun.cpp
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

#include "ChargeGun.hpp"
#include "GameController.hpp"
#include "BaseMapObject.hpp"
#include "common/Player.hpp"
#include "common/StringTokenizer.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"
#include <algorithm>
#include <stdlib.h>
#include <iostream>

using namespace LM;
using namespace std;

ChargeGun::ChargeGun(const char* id, StringTokenizer& gun_data) : Weapon(id) {
	m_last_fired_time = 0;

	m_freeze_time = 0;
	m_damage = 100;
	m_cooldown = 700;
	m_recoil = 1.5;
	m_inaccuracy = 0;
	m_is_continuous = false;
	m_total_ammo = 0;
	m_ammo_recharge = 500;
	m_charge_time = 3000;
	
	m_charged_state = IDLE;
	m_started_charging = 0;

	m_impact_graphic = "shot.png";

	while (gun_data.has_more()) {
		parse_param(gun_data.get_next());
	}

	reset();
}

bool	ChargeGun::parse_param(const char* param_string) {
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
	} else if (strncmp(param_string, "charge_time=", 12) == 0) {
		m_charge_time = atol(param_string + 12);
	} else if (strcmp(param_string, "continuous") == 0) {
		m_is_continuous = true;
	} else if (strcmp(param_string, "notcontinuous") == 0) {
		m_is_continuous = false;
	} else {
		return Weapon::parse_param(param_string);
	}
	return true;
}

void	ChargeGun::fire(Player& player, GameController& gc, Point startpos, double direction) {
	if (m_charged_state == CHARGING) {
		gc.update_cooldown_bar(1.0 - ((get_ticks() - m_started_charging) * 1.0 / m_charge_time));
		if (get_ticks() - m_started_charging > m_charge_time) {
			m_charged_state = CHARGED;
		}
	} else if (m_charged_state == CHARGED) {
		m_charged_state = FIRING;
	}

	if (m_charged_state != FIRING || (m_last_fired_time != 0 && get_ticks() - m_last_fired_time < m_cooldown)) {
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

void	ChargeGun::discharged(Player& player, GameController& gc, StringTokenizer& data) {
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

void	ChargeGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) {
	double	angle;
	data >> angle;

	// Adjust the shot player's velocity
	shot_player.set_velocity(shot_player.get_velocity() - Vector::make_from_magnitude(m_recoil, angle));

	if (has_effect) {
		// Deal damage to the player
		if (!gc.damage(m_damage, &shooting_player)) {
			// Player was not killed, so freeze the player based on this weapon's effects
			gc.freeze(m_freeze_time);
		}
	}
}

/*
void	ChargeGun::select(Player& selecting_player, GameController& gc) {
}
*/

void 	ChargeGun::mouse_button_event(const SDL_Event& event) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (m_charged_state == IDLE) {
			m_charged_state = CHARGING;
			m_started_charging = get_ticks();
		}
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		if (m_charged_state == CHARGING || m_charged_state == FIRING) {
			m_charged_state = IDLE;
			m_started_charging = 0;
		}
	}
}

void	ChargeGun::reset() {
	m_last_fired_time = 0;
	m_current_ammo = m_total_ammo;
	m_charged_state = IDLE;
	m_started_charging = 0;
}

uint64_t	ChargeGun::get_remaining_cooldown() const {
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_cooldown) {
			return m_cooldown - time_since_fire;
		}
	}
	return 0;
}

int	ChargeGun::get_current_ammo () const {
	if (m_last_fired_time && m_ammo_recharge) {
		// Take into account the ammo recharge that has occurred since last firing
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		return min<int>(m_total_ammo, m_current_ammo + time_since_fire / m_ammo_recharge);
	}
	return m_current_ammo;
}

