/*
 * client/GraphicalMap.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GraphicalMap.hpp"
#include "GameWindow.hpp"
#include "MapObject.hpp"
#include "Sprite.hpp"
#include "TiledGraphic.hpp"
#include "common/PacketReader.hpp"
#include <memory>
#include <limits>

using namespace std;

GraphicalMap::GraphicalMap(GameWindow* window) {
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

			string	sprite_path("data/sprites/"); // TODO: don't hard code, really
			sprite_path += sprite_name;
			sprite_path += ".png";

			if (type == OBSTACLE) {
				Sprite*		sprite = new Sprite(sprite_path.c_str());
				map_object.set_sprite(sprite);

				// Bounding polygon - specified by a list of points in the map file
				// The points are converted into a list of lines for internal representation.
				Polygon&	bounding_polygon(map_object.get_bounding_polygon());
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
					bounding_polygon.make_rectangle(sprite->get_image_width(), sprite->get_image_height(), upper_left);
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

			string	sprite_path("data/sprites/"); // TODO: don't hard code, really
			if (team == 'A') {
				sprite_path += "red_gate.png";
				m_gates[0] = &map_object;
			} else if (team == 'B') {
				sprite_path += "blue_gate.png";
				m_gates[1] = &map_object;
			}

			// XXX: This assumes the height of the sprite is 1 pixel.
			Sprite*	sprite = new Sprite(sprite_path.c_str());
			sprite->set_priority(Graphic::BACKGROUND);
			sprite->set_scale_y(double(GATE_HEIGHT));
			map_object.set_sprite(sprite);

			map_object.get_bounding_polygon().make_rectangle(sprite->get_image_width() + GATE_EXTENT * 2, GATE_HEIGHT, upper_left - Point(GATE_EXTENT, 0));
		}

		break;
	}

	if (map_object.has_sprite()) {
		// Register the map object's sprite.
		m_window->register_graphic(map_object.get_sprite());
	}

}

// progress is in [0.0,1.0], where 0 == not lowered .. 1 == lowered all the way
void	GraphicalMap::set_gate_progress(char team, double progress) {
	if (MapObject* object = get_gate_object(team)) {
		if (object->has_sprite()) {
			object->get_sprite()->set_scale_y(GATE_HEIGHT * (1.0 - progress));
		}
	}
}

MapObject* 	GraphicalMap::get_gate_object(char team) {
	if (team == 'A' || team == 'B') {
		return m_gates[team - 'A'];
	} else {
		return NULL;
	}
}

