/*
 * client/Hazard.hpp
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

#ifndef LM_CLIENT_HAZARD_HPP
#define LM_CLIENT_HAZARD_HPP

#include "BaseMapObject.hpp"
#include "MapObjectParams.hpp"
#include "common/Shape.hpp"
#include <memory>
#include <string>
#include <stdint.h>

namespace LM {
	class Hazard : public BaseMapObject {
	private:
		std::string		m_graphic_name;
		MapObjectParams		m_params;
		Graphic*		m_graphic;
		std::auto_ptr<Shape>	m_bounding_shape;
		bool			m_is_slippery;

		char			m_team;			// What team this affects (0 for all players)
		int			m_damage;		// Damage when player interacts with object
		uint64_t		m_damage_rate;		// How often damage is applied (when interacting)
		bool			m_is_collidable;	// Can player collide with this hazard?  If false, then it's a hazard *area*
		int			m_collision_damage;	// Damage when player collides with obstacle
		uint64_t		m_freeze_on_hit;	// How long you're frozen for (only when colliding)
		uint64_t		m_freeze_time;		// How long you're frozen for when your energy is reduced to 0 by this hazard
		double			m_repel_velocity;	// The magnitude of velocity at which killed player should be pushed away
		double			m_bounce_factor;	// How much your velocity changes when you bounce off

		uint64_t		m_last_damage_time;	// 0 if not engaged
		double			m_angle_of_incidence;	// Angle of incidence of the surface we landed on (used for repelling players)

		bool			repel_player(Player&);	// Returns true if player was repelled, false otherwise

	public:
		explicit Hazard (Point pos);

		virtual Graphic*	get_graphic () const { return m_graphic; }
		virtual const Shape*	get_bounding_shape () const { return m_bounding_shape.get(); }

		virtual bool	is_jumpable () const { return m_is_collidable; }
		virtual bool	is_shootable () const { return m_is_collidable; }
		virtual bool	is_collidable () const { return m_is_collidable; }
		virtual bool	is_interactive () const { return true; }
		virtual bool	is_engaged () const { return m_last_damage_time != 0; }
		virtual bool	shot (GameController& gc, Player& shooter, Point point_hit, double direction) { return m_is_collidable; }
		virtual void	collide (GameController& gc, Player& player, Point old_position, double angle_of_incidence);
		virtual void	interact (GameController& gc, Player& player);
		virtual void	disengage (GameController& gc, Player& player);
		virtual void	init (MapReader& reader, ClientMap& map);
	};
}

#endif
