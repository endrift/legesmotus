/*
 * common/StandardGun.hpp
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

#ifndef LM_COMMON_StandardGun_HPP
#define LM_COMMON_StandardGun_HPP

#include "common/Weapon.hpp"
#include "common/StringTokenizer.hpp"
#include "common/Packet.hpp"
#include <stdint.h>
#include <Box2D/Box2D.h>
#include <vector>

#define StandardGun NewStandardGun

class b2World;

namespace LM {
	struct HitData {
		b2Fixture*		fixture;
		b2Vec2			point;
		b2Vec2			normal;
		float32			fraction;
	};

	class StandardGun : public Weapon, public b2RayCastCallback {
	private:
		// Weapon settings
		float			m_max_range;		// What is the maximum range at which the gun is effective?
		uint64_t		m_freeze_time;		// If the gun freezes someone, for how long?
		int 			m_damage;		// How much damage does the gun do?
		uint64_t		m_cooldown;		// How much time must the gun cool down between shots?
		float			m_recoil;		// How much recoil does this gun create?
		float			m_inaccuracy;		// The angle by which firing may deviate randomly
		bool			m_is_continuous;	// Is this gun able to be fired repeatedly by holding down the trigger?
		int			m_total_ammo;		// Ammo the gun can hold, 0 == unlimited
		uint64_t		m_ammo_recharge;	// Give back one unit of ammo this often
		float			m_penetrates_players;	// At what strength does this gun penetrate players (0-1).
		float			m_penetrates_walls; 	// At what strength does this gun penetrate walls (0-1).
		int			m_max_penetration;	// Max number of objects that can be penetrated.
		float			m_damage_degradation;	// Damage decreases by this much per unit distance
		int			m_nbr_projectiles;	// How many projectiles to fire (default, 1)
		float			m_angle;		// Total angle spanned by this weapon (in RADIANS)
		uint64_t		m_freeze_reduction; 	// Time by which this gun reduces the hit player's freeze time.
		unsigned int		m_energy_cost;		// Amount of energy this gun takes from the player when fired.

		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce cooldown)
		int			m_current_ammo;		// How much ammo does the gun have left?
		float			m_current_damage;	// How much damage is the gun currently doing on this shot?
		int			m_objects_penetrated;	// How many objects has the current shot gone through?
		
		// Data for firing/rays:
		std::vector<std::vector<HitData> > m_hit_data;	// Holds the information about each hit.
		float			m_last_fired_dir;	// Holds the last angle at which the gun was fired.

	protected:
		virtual bool		parse_param(const char* param_string);

	public:
		StandardGun(uint32_t id, StringTokenizer& gun_data);

		virtual void		fire(b2World* physics, Player& player, Point start, float direction);
		virtual void		hit(Player* hit_player, const Packet::PlayerHit* p);
		virtual void		hit_object(PhysicsObject* object, Shot* shot, b2Contact* contact) {};
		
		virtual void		reset();
		virtual bool		is_continuous() { return m_is_continuous; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_cooldown; }

		virtual int		get_total_ammo () const { return m_total_ammo; }
		virtual int		get_current_ammo () const;
		
		Packet::PlayerHit* generate_next_hit_packet(Packet::PlayerHit* p, Player* shooter);
		
		// Box2D Physics Callbacks
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	};
}

#endif
