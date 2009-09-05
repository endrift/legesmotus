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

namespace LM {
	class GameController;
	class Player;
	class PacketReader;

	class Weapon {
	public:
		// When the current player (player) fires this weapon:
		//  direction is in radians
		virtual void	fire(Player& player, GameController& gc, Point start, double direction) = 0;
		// When another player (player) fires this weapon:
		virtual void	discharged(Player& player, GameController& gc) = 0;
		// When the current player (player) is hit by this weapon:
		virtual void	hit(Player& player, GameController& gc) = 0;

		virtual const char*	gun_graphic() const = 0;
		virtual const char*	gun_fired_graphic() const = 0;

		static Weapon*	create_weapon (const char* weapon_name, PacketReader& data);
	};
}

#endif
