/*
 * client/FreezeGun.hpp
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

#ifndef LM_CLIENT_FREEZEGUN_HPP
#define LM_CLIENT_FREEZEGUN_HPP

#include "client/Weapon.hpp"
#include <stdint.h>

namespace LM {
	class FreezeGun : public Weapon {
	private:
		// Weapon settings
		uint64_t		m_freeze_time;
		int 			m_damage;
		uint64_t		m_delay;
		double			m_recoil;
		double			m_inaccuracy;		// The angle by which firing may deviate randomly
		bool			m_is_continuous;

		// Weapon state
		uint64_t		m_last_fired_time;	// Time that this gun was last fired (to enforce delay)

	public:
		FreezeGun(const char* name, uint64_t freeze_time, int damage, uint64_t delay, double recoil, double inaccuracy, bool is_continuous);
		explicit FreezeGun(PacketReader& gun_data);

		virtual void		fire(Player& player, GameController& gc, Point start, double direction);
		virtual void		discharged(Player& player, GameController& gc, PacketReader& data);
		virtual void		hit(Player& player, Player& shooting_player, bool has_effect, GameController& gc, PacketReader& data);
		virtual void		reset();
		virtual bool		is_continuous() { return m_is_continuous; }
		virtual uint64_t	get_remaining_cooldown() const;
		virtual uint64_t	get_total_cooldown() const { return m_delay; }

		virtual const char*	gun_graphic() const { return "gun_noshot"; }
		virtual const char*	gun_fired_graphic() const { return "gun_fired"; }
	};
}

#endif
