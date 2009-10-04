/*
 * client/ImpactCannon.hpp
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

#ifndef LM_CLIENT_IMPACTCANNON_HPP
#define LM_CLIENT_IMPACTCANNON_HPP

#include "client/Weapon.hpp"
#include <stdint.h>

namespace LM {
	class ImpactCannon : public Weapon {
	private:
		// Weapon settings
		uint64_t		m_freeze_time;
		int			m_damage;
		double			m_force;
		uint64_t		m_cooldown;
		double			m_recoil;

		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce cooldown)

	protected:
		virtual bool		parse_param(const char* param_string);

	public:
		ImpactCannon(const char* name, uint64_t freeze_time, int damage, double force, uint64_t delay, double recoil);
		ImpactCannon(const char* id, StringTokenizer& gun_data);

		virtual void		fire(Player& player, GameController& gc, Point start, double direction);
		virtual void		discharged(Player& player, GameController& gc, StringTokenizer& data);
		virtual void		hit(Player& player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data);
		//virtual void		select(Player& player, GameController& gc);
		virtual void		reset();
		virtual bool		is_continuous() { return false; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_cooldown; }

		virtual const char*	gun_graphic() const { return "gun_noshot"; }
		virtual const char*	gun_fired_graphic() const { return "gun_fired"; }
	};
}

#endif
