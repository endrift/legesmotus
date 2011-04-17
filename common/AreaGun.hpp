/*
 * common/AreaGun.hpp
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

#ifndef LM_COMMON_AREAGUN_HPP
#define LM_COMMON_AREAGUN_HPP

#include "common/Weapon.hpp"
#include "common/StringTokenizer.hpp"
#include "common/Packet.hpp"
#include <stdint.h>
#include "common/physics.hpp"
#include <vector>
#include "common/PhysicsObject.hpp"
#include "common/Point.hpp"

namespace LM {
	struct HitPlayer {
		Player* player;
		float angle;
		Point point;
	};

	class AreaGun : public Weapon {
	private:
		// Weapon settings
		uint64_t		m_freeze_on_hit;	// If the gun hits someone, for how long does it freeze them each time?
		uint64_t		m_freeze_time;		// If the gun freezes someone, for how long?
		int 			m_damage;		// How much damage does the gun do?
		uint64_t		m_cooldown;		// How much time must the gun cool down between shots?
		float			m_recoil;		// How much recoil does this gun create?
		float			m_force;		// How much force is generated on the target?
		bool			m_is_continuous;	// Is this gun able to be fired repeatedly by holding down the trigger?
		int			m_total_ammo;		// Ammo the gun can hold, 0 == unlimited
		uint64_t		m_ammo_recharge;	// Give back one unit of ammo this often
		float			m_damage_degradation;	// Damage decreases by this much per unit distance
		unsigned int		m_energy_cost;		// Amount of energy this gun takes from the player when fired.
		b2Shape*		m_area;			// Area affected by the shot
		float			m_max_range;		// Maximum range at which to test for hits
	
		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce cooldown)
		int			m_current_ammo;		// How much ammo does the gun have left?
		Player*			m_firing_player;	// Which player fired this weapon?
		Shot*			m_shot;			// The current shot that was fired.
		std::vector<HitPlayer>	m_hits;			// A list of players that were hit.
		
		// For ray casts:
		uint32_t		m_hit_player_check;	// The ID of the closest player hit by the ray, -1 if none or if a wall is in the way.
		float			m_shortest_dist;	// The shortest distance seen in this ray cast.
	
		Point			m_pivot;		// The pivot point and the point where damage spreads from
		
		b2World*		m_physics;
	
	protected:
		virtual bool		parse_param(const char* param_string);

	public:
		AreaGun(uint32_t id);
		
		virtual Packet::WeaponDischarged* fire(b2World* physics, Player& player, Point start, float direction, Packet::WeaponDischarged* packet);
		virtual void		was_fired(b2World* physics, Player& player, float direction);
		virtual void		hit(Player* hit_player, Player* firing_player, const Packet::PlayerHit* p);
		virtual void		hit_object(PhysicsObject* object, Shot* shot, b2Contact* contact);
		
		virtual void		reset();
		virtual bool		is_continuous() { return m_is_continuous; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_cooldown; }

		virtual int		get_total_ammo () const { return m_total_ammo; }
		virtual int		get_current_ammo () const;
		
		virtual int		get_damage() const;
		virtual int		get_damage_at_point(int start_x, int start_y, int target_x, int target_y) const;
		virtual float		get_base_force() const;
		virtual float		get_force(int start_x, int start_y, int target_x, int target_y) const;
		
		virtual float		get_freeze_time() const;
		
		Packet::PlayerHit* generate_next_hit_packet(Packet::PlayerHit* p, Player* shooter);
	};
}

#endif
