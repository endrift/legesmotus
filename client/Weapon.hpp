/*
 * client/Weapon.hpp
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

#ifndef LM_CLIENT_WEAPON_HPP
#define LM_CLIENT_WEAPON_HPP

#include "common/Point.hpp"
#include <string>
#include <stdint.h>

namespace LM {
	class GameController;
	class Player;
	class StringTokenizer;
	class WeaponReader;

	// TODO: need proper graphics support, a la MapObjectParams
	class Weapon {
	private:
		std::string		m_id;			// Internal name

	protected:
		std::string		m_name;			// Friendly name - presented to user
		std::string		m_hud_graphic;		// Name of graphic that is used in HUD/UI
		std::string		m_normal_graphic_info;	// Graphic info when player is not firing
		std::string		m_firing_graphic_info;	// Graphic info when player is firing
	
		virtual bool		parse_param(const char* param_data);

	public:
		Weapon();
		explicit Weapon(const char* id);
		virtual ~Weapon() { }

		const char*		get_id() const { return m_id.c_str(); }
		const char*		get_name() const { return m_name.c_str(); }
		const char*		hud_graphic() const { return m_hud_graphic.c_str(); }

		// When the current player (player) fires this weapon:
		//  direction is in radians
		virtual void		fire(Player& player, GameController& gc, Point start, double direction) = 0;
		// When another player (player) fires this weapon:
		virtual void		discharged(Player& player, GameController& gc, StringTokenizer& data) = 0;
		// When the current player (player) is hit by this weapon:
		virtual void		hit(Player& player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data) = 0;
		// Call when a player selects this weapon:
		virtual void		select(Player& player, GameController& gc);
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

		static Weapon*		new_weapon (WeaponReader&);
	};
}

#endif
