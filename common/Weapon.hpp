/*
 * common/Weapon.hpp
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

#ifndef LM_COMMON_WEAPON_HPP
#define LM_COMMON_WEAPON_HPP

#include "common/Point.hpp"
#include "common/Packet.hpp"
#include "common/PhysicsObject.hpp"
#include "common/Shot.hpp"
#include <string>
#include <stdint.h>

class b2World;
class b2Contact;

namespace LM {
	class Player;
	class StringTokenizer;
	class WeaponReader;
	class ClientWeapon;

	class Weapon {
	private:
		long			m_id;			// Internal name

	protected:
		std::string		m_name;			// Friendly name - presented to user
		ClientWeapon*	m_clientpart;
	
		virtual bool		parse_param(const char* param_data);

	public:
		Weapon();
		explicit Weapon(uint32_t id);
		virtual ~Weapon();

		const long		get_id() const { return m_id; }
		const char*		get_name() const { return m_name.c_str(); }

		// TODO: Do we want these to be in the common Weapon, or just the client one?
		// When the current player (player) fires this weapon:
		//  direction is in radians
		virtual void		fire(b2World* physics, Player& player, Point start, float direction) = 0;
		// When another player (player) fires this weapon:
		//virtual void		discharged(Player& player, StringTokenizer& data) = 0;
		// When the current player (player) is hit by this weapon:
		virtual void		hit(Player* hit_player, const Packet::PlayerHit* p) = 0;
		
		// Called when another object sees that a shot owned by this gun has hit something.
		virtual void		hit_object(PhysicsObject* object, Shot* shot, b2Contact* contact) = 0;
		
		virtual Packet::PlayerHit*	generate_next_hit_packet(Packet::PlayerHit* p, Player* shooter) = 0;
		
		// Call when a player selects this weapon:
		virtual void		select(Player* player);
		// Call when the round ends to reset this weapon's state:
		virtual void		reset() = 0;

		// Can this gun be continuously fired?
		virtual bool		is_continuous() = 0;

		// How long until this weapon can be fired again?
		virtual uint64_t	get_remaining_cooldown() const = 0;
		virtual uint64_t	get_total_cooldown() const = 0;

		virtual int		get_total_ammo () const { return 0; }
		virtual int		get_current_ammo () const { return 0; }
		virtual bool		has_limited_ammo () const { return get_total_ammo() != 0; }
		
		// TODO: set up mouse events?

		static Weapon*		new_weapon (WeaponReader&, ClientWeapon* clientpart = NULL);
	};
}


#endif
