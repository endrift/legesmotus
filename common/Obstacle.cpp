/*
 * common/Obstacle.cpp
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

#include "Obstacle.hpp"
#include "ClientMapObject.hpp"
#include "MapReader.hpp"

using namespace LM;
using namespace std;

Obstacle::Obstacle (Point position, ClientMapObject* clientpart) : MapObject(position, clientpart) {
	// TODO
	m_is_slippery = false;
	m_bounce_factor = 0.9;
}

void	Obstacle::collide(GameLogic* logic, Player* player, Point old_position, float angle_of_incidence) {
	// TODO
}

void	Obstacle::init(MapReader* reader) {
	MapObject::init(reader);

	string bounding_shape;

	(*reader) >> bounding_shape;

	while (reader->has_more()) {
		string param_string;
		(*reader) >> param_string;

		if (param_string == "slippery") {
			m_is_slippery = true;
		} else if (param_string == "sticky") {
			m_is_slippery = false;
		} else if (strncmp(param_string.c_str(), "bounce=", 7) == 0) {
			m_bounce_factor = atof(param_string.c_str() + 7);
		} else {
			parse_param(param_string.c_str());
		}
	}

	m_bounding_shape.reset(make_bounding_shape(bounding_shape, get_position()));
}
