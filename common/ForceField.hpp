/*
 * common/ForceField.hpp
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

#ifndef LM_COMMON_FORCEFIELD_HPP
#define LM_COMMON_FORCEFIELD_HPP

#include "MapObject.hpp"
#include "common/Shape.hpp"
#include <memory>
#include <stdint.h>
#include "common/misc.hpp"

#define ForceField NewForceField

class b2Body;
class b2World;
class b2Shape;
class b2Contact;

namespace LM {
	class PhysicsObject;

	class ForceField : public MapObject {
	private:
		b2Shape*		m_bounding_shape;
		b2Body*			m_physics_body; 	// Box2D physics body for this map object
		
		bool			m_is_hazardous;		// Is this force field hazardous?
		char			m_team;			// What team this affects (0 for all players)
		int			m_damage;		// Damage when player interacts with object
		uint64_t		m_damage_rate;		// How often damage is applied (when interacting)
		uint64_t		m_freeze_time;		// How long you're frozen for when your energy is reduced to 0 by this hazard
		uint64_t		m_last_damage_time;	// 0 if not engaged
		
		float			m_force;		// The magnitude of the field's constant force
		float			m_force_dir;		// The angle in which the force pushes (in radians)
		float			m_friction;		// The magnitude of the field's friction
		float			m_decay;		// How much does this force decay over distance?
		bool			m_is_directional; 	// Is this force always in the same direction, or radial from the center?
		bool			m_is_repulsive; 	// Should the force attract or repel?
		bool			m_affects_frozen; 	// Does this force affect frozen players?
		bool			m_affects_thawed; 	// Does this force affect unfrozen players?
		bool			m_sucks_off_obstacles; 	// Is this force strong enough to detach players from obstacles?

	public:
		explicit ForceField(Point pos, ClientMapObject* clientpart = NULL);

		// Tells this object to initialize a new physics body in a given world.
		virtual void initialize_physics(b2World* world);

		virtual const b2Shape* get_bounding_shape () const { return m_bounding_shape; }

		virtual bool is_jumpable() const { return false; }
		virtual bool is_shootable() const { return false; }
		virtual bool is_collidable() const { return false; }
		virtual bool is_interactive() const { return true; }
		virtual bool is_engaged() const { return false; }
		virtual bool shot(Player* shooter, Point point_hit, float direction) { return false; }
		
		virtual CollisionResult get_collision_result(PhysicsObject* other, b2Contact* contact) { return IGNORE; }
		virtual CollisionResult collide(PhysicsObject* other, b2Contact* contact) { return IGNORE; }
		virtual void interact(PhysicsObject* other, b2Contact* contact);
		virtual void disengage(PhysicsObject* other) { m_last_damage_time = 0; }
		virtual void init(MapReader* reader);
	};
}

#endif
