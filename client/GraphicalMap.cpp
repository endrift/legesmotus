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
#include "common/PacketReader.hpp"
#include <memory>
#include <limits>

using namespace std;

GraphicalMap::GraphicalMap(GameWindow* window) {
	m_window = window;
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
		{
			string	sprite_name;
			object_data >> sprite_name;

			string	sprite_path("data/sprites/"); // TODO: don't hard code, really
			sprite_path += sprite_name;
			sprite_path += ".png";

			map_object.set_sprite(new Sprite(sprite_path.c_str()));

			// TODO: set bounding polygon
	
		}
		break;
	case GATE:
		{
			// TODO: will gates have standard sprites/polygons?
			char	team;
			object_data >> team;
			map_object.set_team(team);
		}

		break;
	case SPAWN_POINT:
	
		break;
	}

	if (map_object.has_sprite()) {
		// Register the map object's sprite.
		Sprite*	sprite = map_object.get_sprite();
		//sprite->set_priority(numeric_limits<int>::max()); // TODO: should have standard enums in Graphic class
		m_window->register_graphic(sprite);
	}
}

