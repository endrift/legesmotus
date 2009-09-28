/*
 * client/StopGun.cpp
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

#include "StopGun.hpp"
#include "GameController.hpp"
#include "BaseMapObject.hpp"
#include "common/Player.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/timer.hpp"
#include "common/math.hpp"

using namespace LM;
using namespace std;

StopGun::StopGun(const char* name, int damage, uint64_t delay, double recoil) : Weapon(name) {
	m_last_fired_time = 0;
	m_damage = damage;
	m_delay = delay;
	m_recoil = recoil;
}

StopGun::StopGun(PacketReader& gun_data) {
	m_last_fired_time = 0;
	string		name;
	gun_data >> name >> m_damage >> m_delay >> m_recoil;
	set_name(name.c_str());
}

void	StopGun::fire(Player& player, GameController& gc, Point startpos, double direction) {
	if (m_last_fired_time != 0 && get_ticks() - m_last_fired_time < m_delay) {
		// Firing too soon
		return;
	}

	// Cause recoil if the player is not hanging onto a wall.
	if (!player.is_grabbing_obstacle()) {
		player.set_velocity(player.get_velocity() - Vector::make_from_magnitude(m_recoil, direction));
	}

	m_last_fired_time = get_ticks();
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
	fired_packet << player.get_id() << get_name() << startpos << direction;
	if (hit_point.x != -1 && hit_point.y != -1) {
		fired_packet << hit_point;
		gc.show_bullet_impact(hit_point);
	}
	gc.send_packet(fired_packet);
	
	// If a player was hit, send a packet about it
	if (hit_player != NULL) {
		if (!hit_player->is_frozen()) {
			gc.play_sound("hit");
		}

		PacketWriter	hit_packet(PLAYER_HIT_PACKET);
		hit_packet << player.get_id() << get_name() << hit_player->get_id() << direction - M_PI;
		gc.send_packet(hit_packet);
	}
}

void	StopGun::discharged(Player& player, GameController& gc, PacketReader& data) {
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

		gc.show_bullet_impact(endpos);
	}
}

void	StopGun::hit(Player& shot_player, Player& shooting_player, bool has_effect, GameController& gc, PacketReader& data) {
	double	angle;
	data >> angle;

	if (has_effect) {
		// Deal damage to the player
		gc.damage(m_damage, &shooting_player);

		// Set velocity to 0
		shot_player.set_velocity(Vector(0, 0));
	}
}

void	StopGun::reset() {
	m_last_fired_time = 0;
}

void	StopGun::select(Player& selecting_player, GameController& gc) {
}

uint64_t	StopGun::get_remaining_cooldown() const
{
	if (m_last_fired_time) {
		uint64_t	time_since_fire = get_ticks() - m_last_fired_time;
		if (time_since_fire < m_delay) {
			return m_delay - time_since_fire;
		}
	}
	return 0;
}

