/*
 * common/Obstacle.hpp
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

#ifndef LM_COMMON_OBSTACLE_HPP
#define LM_COMMON_OBSTACLE_HPP

#include "MapObject.hpp"
#include "common/Shape.hpp"
#include <memory>

#define Obstacle NewObstacle

class b2Body;
class b2World;
class b2Shape;

namespace LM {
	class Obstacle : public MapObject {
	private:
		b2Shape*		m_bounding_shape;
		bool			m_is_slippery;
		double			m_bounce_factor;
		b2Body*			m_physics_body; // Box2D physics body for this map object

	public:
		explicit Obstacle(Point pos, ClientMapObject* clientpart = NULL);

		// Tells this object to initialize a new physics body in a given world.
		virtual void initialize_physics(b2World* world);

		virtual const b2Shape* get_bounding_shape () const { return m_bounding_shape; }

		virtual bool is_jumpable() const { return true; }
		virtual bool is_shootable() const { return true; }
		virtual bool is_collidable() const { return true; }
		virtual bool is_interactive() const { return false; }
		virtual bool is_engaged() const { return false; }
		virtual bool shot(GameLogic* logic, Player* shooter, Point point_hit, float direction) { return true; }
		virtual void collide(GameLogic* logic, Player* player, Point old_position, float angle_of_incidence);
		virtual void interact(GameLogic* logic, Player* player) { }
		virtual void disengage(GameLogic* logic, Player* player) { }
		virtual void init(MapReader* reader);
	};
}

#endif
