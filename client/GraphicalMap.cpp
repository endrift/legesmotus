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

using namespace std;

GraphicalMap::GraphicalMap(GameWindow* window) {
	m_window = window;
}

GraphicalMap::~GraphicalMap() {
	clear();
}

void	GraphicalMap::clear() {
	for (list<MapObject>::iterator it(m_objects.begin()); it != m_objects.end(); ++it) {
		m_window->unregister_graphic(it->get_sprite());
		delete it->get_sprite();
	}
	Map::clear();
}

void	GraphicalMap::add_object(PacketReader& object_data) {
	int			type = object_data.packet_type();
	auto_ptr<Sprite>	sprite;
	Point			upper_left;
	object_data >> upper_left;

	switch (type) {
	case OBSTACLE:
		sprite.reset(new Sprite("data/sprites/blue_full.png"));
	
		break;
	case GATE:

		break;
	case SPAWN_POINT:
	
		break;
	}

	if (sprite.get()) {
		sprite->set_x(upper_left.x);
		sprite->set_y(upper_left.y);
		m_window->register_graphic(sprite.get());
		m_objects.push_back(MapObject(type, sprite.get(), upper_left));

		sprite.release(); // It's now the responsibility of clear() to delete sprite.
	}
}

