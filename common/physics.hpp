/*
 * common/physics.hpp
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

#ifndef LM_COMMON_PHYSICS_HPP
#define LM_COMMON_PHYSICS_HPP

#include <Box2D/Box2D.h>
#include "common/Point.hpp"
#include "common/math.hpp"

namespace LM {
	extern const float PHYSICS_TO_GAME;
	extern const float GAME_TO_PHYSICS;

	// Make a Box2D shape from a string
	b2Shape* make_shape_from_string(const std::string& shape_string, float scale_x = 1.0f, float scale_y = 1.0f);

	// Go from game to physics "world" units
	inline float to_physics(float value) { return value * GAME_TO_PHYSICS; }
	inline float to_game(float value) { return value * PHYSICS_TO_GAME; }
}

#endif
