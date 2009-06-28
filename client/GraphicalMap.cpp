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
#include "common/MapReader.hpp"
#include "common/PathManager.hpp"
#include "common/team.hpp"
#include <memory>
#include <limits>
#include <cstring>

using namespace LM;
using namespace std;

GraphicalMap::GraphicalMap(PathManager& path_manager, GameWindow* window) : m_path_manager(path_manager) {
	m_window = window;
	m_gates[0] = m_gates[1] = NULL;
}

GraphicalMap::~GraphicalMap() {
	clear();
}

void	GraphicalMap::clear() {
	m_cached_graphics.clear();
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

template<class T> T* GraphicalMap::load_graphic(const std::string& sprite_name) {
	Graphic*&	graphic = m_cached_graphics[sprite_name];

	if (!graphic) {
		if (sprite_name.find_first_of("/\\") != string::npos) {
			// TODO: do this more nicely
			return new T(static_cast<SDL_Surface*>(NULL));
		}

		// Graphic not cached yet.
		string	filename(sprite_name);
		filename += ".png";

		T*	new_graphic = new T(m_path_manager.data_path(filename.c_str(), "sprites"));
		graphic = new_graphic;
		return new_graphic;
	}

	// Create this new graphic from the cached graphic.
	return new T(*graphic);
}

void	GraphicalMap::add_object(MapReader& object_data) {
	ObjectType			type(object_data.get_type());
	Point				upper_left;
	object_data >> upper_left;

	// Append it to the list of objects.
	m_objects.push_back(MapObject(type, upper_left));
	// Use this reference to do more stuff (i.e. set the sprite and bounding polygon) with the new object.
	MapObject&			map_object(m_objects.back());

	switch (type) {
	case SPRITE:
		{
			string		flags;
			string		sprite_name;
			object_data >> flags >> sprite_name;

			bool		is_tiled = flags.find('T') != string::npos;
			bool		is_obstacle = flags.find('O') != string::npos;
			int		width = 0;
			int		height = 0;

			if (is_tiled) {
				// Tiled graphic
				TiledGraphic* background = load_graphic<TiledGraphic>(sprite_name);
				map_object.set_sprite(background);

				// Also have to parse out tile width, tile height, and (optionally) tile start point
				object_data >> width >> height;
				background->set_width(width);
				background->set_height(height);

				if (object_data.has_more()) {
					Point		start_point;
					object_data >> start_point;

					background->set_start_x(start_point.x);
					background->set_start_y(start_point.y);
				}
			} else {
				Sprite*	sprite = load_graphic<Sprite>(sprite_name);
				width = int(sprite->get_image_width());
				height = int(sprite->get_image_height());
				map_object.set_sprite(sprite);
			}

			Graphic*	sprite = map_object.get_sprite();

			if (is_obstacle) {
				// Bounding polygon - specified by a list of points in the map file
				// The points are converted into a list of lines for internal representation.
				Polygon&	bounding_polygon(map_object.get_bounding_polygon());
				if (object_data.has_more()) {
					StringTokenizer	tokenizer(object_data.get_next(), ';');

					Point		first_point;
					tokenizer >> first_point;

					Point		previous_point(first_point);
					while (tokenizer.has_more()) {
						Point	next_point;
						tokenizer >> next_point;
						bounding_polygon.add_line(upper_left + previous_point, upper_left + next_point); // Translate to be relative to the upper left of the map

						previous_point = next_point;
					}
					bounding_polygon.add_line(upper_left + previous_point, upper_left + first_point); // Translate to be relative to the upper left of the map
				} else {
					// If no points were specified in the file, assume it's a rectangle representing the width and height of the sprite.
					bounding_polygon.make_rectangle(width, height, upper_left);
				}

				sprite->set_priority(Graphic::OBSTACLE);
			} else {
				sprite->set_priority(Graphic::BACKGROUND);
			}
		}
		break;
	case GATE:
		{
			string		team_string;
			object_data >> team_string;

			char		team = parse_team_string(team_string.c_str());

			map_object.set_team(team);

			string	sprite_name;
			if (team == 'A') {
				sprite_name = "blue_gate";
				m_gates[0] = &map_object;
			} else if (team == 'B') {
				sprite_name = "red_gate";
				m_gates[1] = &map_object;
			}

			// XXX: This assumes the height of the sprite is 1 pixel.
			Sprite*	sprite = load_graphic<Sprite>(sprite_name);
			sprite->set_priority(Graphic::OBSTACLE);
			sprite->set_scale_y(double(GATE_HEIGHT));
			sprite->set_antialiasing(false);
			map_object.set_sprite(sprite);

			map_object.get_bounding_polygon().make_rectangle(int(sprite->get_image_width()) + GATE_EXTENT * 2, GATE_HEIGHT, upper_left - Point(GATE_EXTENT, 0)); // XXX: casting double to int here
		}

		break;
	default:
		break;
	}

	if (map_object.has_sprite()) {
		// Register the map object's sprite.
		m_window->register_graphic(map_object.get_sprite());
	}

}

bool	GraphicalMap::load(istream& in) {
	// Clear the graphics cache before and after loading the map.
	// This is to avoid possible memory management issues where a map object is removed after loading, but a dangling pointer to the graphic is left in m_cached_graphics.
	// Graphics caching is really only needed during initial map loading to make a significant difference anyways.
	m_cached_graphics.clear();
	bool	result = Map::load(in);
	m_cached_graphics.clear();
	return result;
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

void	GraphicalMap::reset() {
	reset_gates();
}

