/*
 * common/Gate.hpp
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

#ifndef LM_COMMON_GATE_HPP
#define LM_COMMON_GATE_HPP

#include "MapObject.hpp"
#include <map>
#include <stdint.h>
#include "timer.hpp"

class b2Body;
class b2World;
class b2Shape;
class b2Contact;

namespace LM {
	class ClientMap;
	class PhysicsObject;

	class Gate : public MapObject {
	private:
		char			m_team;
		float			m_width;
		float			m_length;
		float			m_extent;
		float			m_rotation;

		float			m_progress;

		bool			m_is_engaged;
		std::map<const Player*, uint64_t> m_engaging_players;
		b2Shape*		m_bounding_shape;
		
		b2Body*			m_physics_body; // Box2D physics body for this map object

	public:
		Gate(Point pos, ClientMapObject* clientpart = NULL);
	
		virtual const b2Shape* get_bounding_shape() const { return m_bounding_shape; }
		
		char get_team() const { return m_team; }
		
		void set_progress(float progress);
		float get_progress() const;

		float get_length() const { return m_length; }
		float get_width() const { return m_width; }
		
		// Decorations have no physics of their own.
		virtual void initialize_physics(b2World* world);
	
		virtual bool is_jumpable() const { return false; }
		virtual bool is_shootable() const { return false; }
		virtual bool is_collidable() const { return false; }
		virtual bool is_interactive() const { return true; }
		virtual bool is_engaged() const { return m_is_engaged; };
		virtual bool is_engaged_by(const Player* player) const { return m_engaging_players.find(player) != m_engaging_players.end(); };
		virtual bool shot(Player* shooter, Point point_hit, float direction) { return false; }
		virtual CollisionResult get_collision_result(PhysicsObject* other, b2Contact* contact);
		virtual CollisionResult collide(PhysicsObject* other, b2Contact* contact);
		virtual void interact(PhysicsObject* other, b2Contact* contact) {}
		virtual void disengage(PhysicsObject* other);
		virtual void init(MapReader* reader);
	};
}
#endif
