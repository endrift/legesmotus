/*
 * client/MapObjectParams.cpp
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

#include "MapObjectParams.hpp"
#include <cstring>
#include <cstdlib>

using namespace LM;
using namespace std;

MapObjectParams::MapObjectParams() {
	is_tiled = false;
	is_antialiased = true;
	priority = 0;
	scale_x = 1.0;
	scale_y = 1.0;
	rotation = 0.0;
}

bool	MapObjectParams::parse(const char* option_string) {
	if (strncmp(option_string, "tile=", 5) == 0) {
		is_tiled = true;
		tile_dimensions = Vector::make_from_string(option_string + 5);
	} else if (strcmp(option_string, "notile") == 0) {
		is_tiled = false;
	} else if (strncmp(option_string, "scale=", 6) == 0) {
		scale_x = scale_y = atof(option_string + 6);
	} else if (strncmp(option_string, "rotate=", 7) == 0) {
		rotation = atof(option_string + 7);
	} else {
		return false;
	}

	return true;
}

