/*
 * client/AreaGun.hpp
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

#ifndef LM_CLIENT_AREAGUN_HPP
#define LM_CLIENT_AREAGUN_HPP

#include "client/Weapon.hpp"
#include "common/Polygon.hpp"
#include <stdint.h>

namespace LM {
	class AreaGun : public Weapon {
	private:
		// Weapon settings
		double			m_force;
		uint64_t		m_freeze_time;
		int 			m_damage;		// Damage at origin
		double			m_damage_degradation;	// Damage decreases by this much per unit distance
		uint64_t		m_cooldown;		// Minimum time between firing
		double			m_recoil;
		Shape*			m_area;			// Area affected by the shot
		Point			m_pivot;		// The pivot point and the point where damage spreads from
		//bool			m_move_shot;		// Does the pivot move to the point of impact or stay with
								// the shooting player?

		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce cooldown)

	protected:
		virtual bool		parse_param(const char* param_string);

	public:
		AreaGun(const char* id, StringTokenizer& gun_data);
		~AreaGun();

		virtual void		fire(Player& player, GameController& gc, Point start, double direction);
		virtual void		discharged(Player& player, GameController& gc, StringTokenizer& data);
		virtual void		hit(Player& player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data);
		//virtual void		select(Player& player, GameController& gc);
		virtual void		reset();
		virtual bool		is_continuous() { return false; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_cooldown; }
	};
}

#endif
