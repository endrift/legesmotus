/*
 * client/PenetrationGun.hpp
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

#ifndef LM_CLIENT_PENETRATIONGUN_HPP
#define LM_CLIENT_PENETRATIONGUN_HPP

#include "client/Weapon.hpp"
#include <stdint.h>

namespace LM {
	class PenetrationGun : public Weapon {
	private:
		// Weapon settings
		uint64_t		m_freeze_time;
		int 			m_damage;
		uint64_t		m_cooldown;
		double			m_recoil;
		double			m_inaccuracy;		// The angle by which firing may deviate randomly
		bool			m_is_continuous;
		int			m_total_ammo;		// 0 == unlimited
		uint64_t		m_ammo_recharge;	// Give back one unit of ammo this often
		float			m_penetrates_players;	// At what strength does this gun penetrate players (0-1).
		float			m_penetrates_walls; 	// At what strength does this gun penetrate walls (0-1).
		int			m_max_penetration;	// Max number of objects that can be penetrated.

		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce cooldown)
		int			m_current_ammo;

	protected:
		virtual bool		parse_param(const char* param_string);

	public:
		PenetrationGun(const char* id, StringTokenizer& gun_data);

		virtual void		fire(Player& player, GameController& gc, Point start, double direction);
		virtual void		discharged(Player& player, GameController& gc, StringTokenizer& data);
		virtual void		hit(Player& player, Player& shooting_player, bool has_effect, GameController& gc, StringTokenizer& data);
		//virtual void		select(Player& player, GameController& gc);
		virtual void		reset();
		virtual bool		is_continuous() { return m_is_continuous; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_cooldown; }

		virtual int		get_total_ammo () const { return m_total_ammo; }
		virtual int		get_current_ammo () const;
	};
}

#endif
