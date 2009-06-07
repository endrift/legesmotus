/*
 * client/GraphicalMap.cpp
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

#include "GraphicalMap.hpp"
#include "GameWindow.hpp"
#include "MapObject.hpp"
#include "Sprite.hpp"
#include "TiledGraphic.hpp"
#include "common/PacketReader.hpp"
#include "common/PathManager.hpp"
#include "common/team.hpp"
#include <memory>
#include <limits>

using namespace std;

GraphicalMap::GraphicalMap(PathManager& path_manager, GameWindow* window) : m_path_manager(path_manager) {
	m_window = window;
	m_gates[0] = m_gates[1] = NULL;
}

GraphicalMap::~GraphicalMap() {
	clear();
}

void	GraphicalMap::clear() {
	for (list<MapObject>::iterator it(m_objects.begin()); it != m_objects.end(); ++it) {
		if (it->has_sprite()) {
			m_window->unregister_graphic(it->get_sprite());
		}
		delete it->get_sprite();
	}
	m_objects.clear();
	m_gates[0] = m_gates[1] = NULL;
	Map::clear();
}

void	GraphicalMap::add_object(PacketReader& object_data) {
	int			type = object_data.packet_type();
	Point			upper_left;
	object_data >> upper_left;

	// Append it to the list of objects.
	m_objects.push_back(MapObject(type, upper_left));
	// Use this reference to do more stuff (i.e. set the sprite and bounding polygon) with the new object.
	MapObject&		map_object(m_objects.back());

	switch (type) {
	case OBSTACLE:
	case DECORATION:
	case BACKGROUND:
		{
			string	sprite_name;
			object_data >> sprite_name;
			sprite_name += ".png";

			string	sprite_path(m_path_manager.data_path(sprite_name.c_str(), "sprites"));

			if (type == OBSTACLE) {
				Sprite*		sprite = new Sprite(sprite_path.c_str());
				map_object.set_sprite(sprite);

				// Bounding polygon - specified by a list of points in the map file
				// The points are converted into a list of lines for internal representation.
				LMPolygon&	bounding_polygon(map_object.get_bounding_polygon());
				if (object_data.has_more()) {
					Point		first_point;
					object_data >> first_point;

					Point		previous_point(first_point);
					while (object_data.has_more()) {
						Point	next_point;
						object_data >> next_point;
						bounding_polygon.add_line(upper_left + previous_point, upper_left + next_point); // Translate to be relative to the upper left of the map

						previous_point = next_point;
					}
					bounding_polygon.add_line(upper_left + previous_point, upper_left + first_point); // Translate to be relative to the upper left of the map
				} else {
					// If no points were specified in the file, assume it's a rectangle representing the width and height of the sprite.
					bounding_polygon.make_rectangle(int(sprite->get_image_width()), int(sprite->get_image_height()), upper_left); // XXX: casting double to int here
				}

				sprite->set_priority(Graphic::OBSTACLE);

			} else if (type == DECORATION) {
				Sprite*		sprite = new Sprite(sprite_path.c_str());
				sprite->set_priority(Graphic::BACKGROUND);
				map_object.set_sprite(sprite);

			} else if (type == BACKGROUND) {
				TiledGraphic*	background = new TiledGraphic(sprite_path.c_str());
				map_object.set_sprite(background);

				int		width;
				int		height;
				object_data >> width >> height;
				background->set_width(width);
				background->set_height(height);

				if (object_data.has_more()) {
					Point		start_point;
					object_data >> start_point;

					background->set_start_x(start_point.x);
					background->set_start_y(start_point.y);
				}

				background->set_priority(Graphic::BACKGROUND);
			}
		}
		break;
	case GATE:
		{
			char	team;
			object_data >> team;

			map_object.set_team(team);

			string	sprite_path;
			if (team == 'B') {
				sprite_path = m_path_manager.data_path("red_gate.png", "sprites");
				m_gates[1] = &map_object;
			} else if (team == 'A') {
				sprite_path = m_path_manager.data_path("blue_gate.png", "sprites");
				m_gates[0] = &map_object;
			}

			// XXX: This assumes the height of the sprite is 1 pixel.
			Sprite*	sprite = new Sprite(sprite_path.c_str());
			sprite->set_priority(Graphic::BACKGROUND);
			sprite->set_scale_y(double(GATE_HEIGHT));
			sprite->set_antialiasing(false);
			map_object.set_sprite(sprite);

			map_object.get_bounding_polygon().make_rectangle(int(sprite->get_image_width()) + GATE_EXTENT * 2, GATE_HEIGHT, upper_left - Point(GATE_EXTENT, 0)); // XXX: casting double to int here
		}

		break;
	}

	if (map_object.has_sprite()) {
		// Register the map object's sprite.
		m_window->register_graphic(map_object.get_sprite());
	}

}

// progress is in [0.0,1.0], where 0 == closed .. 1 == fully open
void	GraphicalMap::set_gate_progress(char team, double progress) {
	if (MapObject* object = get_gate_object(team)) {
		if (object->has_sprite()) {
			object->get_sprite()->set_scale_y(GATE_HEIGHT * (1.0 - progress));
		}
	}
}

void	GraphicalMap::set_visible(bool visible) {
	for (list<MapObject>::iterator it(m_objects.begin()); it != m_objects.end(); ++it) {
		if (it->has_sprite()) {
			it->get_sprite()->set_invisible(!visible);
		}
	}
}

MapObject* 	GraphicalMap::get_gate_object(char team) {
	if (is_valid_team(team)) {
		return m_gates[team - 'A'];
	} else {
		return NULL;
	}
}

void	GraphicalMap::reset_gates() {
	set_gate_progress('A', 0.0);
	set_gate_progress('B', 0.0);
}

