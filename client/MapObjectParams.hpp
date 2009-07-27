/*
 * client/MapObjectParams.hpp
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

#ifndef LM_CLIENT_MAPOBJECTPARAMS_HPP
#define LM_CLIENT_MAPOBJECTPARAMS_HPP

#include "common/Point.hpp"

namespace LM {
	class MapObjectParams {
	public:
		bool		is_tiled;
		Vector		tile_dimensions;
		bool		is_antialiased;
		int		priority;
		double		scale_x;
		double		scale_y;
		double		rotation;		// in degrees

		MapObjectParams();

		// Parses the string of the form:
		//  tile=width,height
		//  notile
		//  scale=factor
		//  rotate=degrees
		// And saves into this record
		// Returns false if the option string isn't recognized
		bool		parse(const char* option_string);
	};
}

#endif
