/*
 * common/Shot.hpp
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

#ifndef LM_COMMON_SHOT_HPP
#define LM_COMMON_SHOT_HPP

#include "common/PhysicsObject.hpp"
#include "common/Point.hpp"

class b2Body;

namespace LM {
	class Player;
	class Weapon;
	
	class Shot : public PhysicsObject {
	private:
		Weapon* m_weapon;
		Player* m_firing_player;
		b2Body* m_physics_body;
		Point	m_center;

	public:
		Shot();
		Shot(Weapon* weapon, Player* firing_player);
		virtual ~Shot() {};
	
		Weapon* get_weapon() const { return m_weapon; };
		Player* get_firing_player() const { return m_firing_player; };
		b2Body* get_physics_body() const { return m_physics_body; };
		Point	get_center() const { return m_center; };
		
		void set_physics_body(b2Body* body) { m_physics_body = body; };
		void set_center(Point center) { m_center = center; };
	
		virtual ObjectType get_type() const { return SHOT; };
	};
}

#endif
