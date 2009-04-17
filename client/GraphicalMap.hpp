/*
 * client/GraphicalMap.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHICALMAP_HPP
#define LM_CLIENT_GRAPHICALMAP_HPP

#include "common/Map.hpp"
#include "client/MapObject.hpp"
#include <list>

class GameWindow;

/*
 * Derives from Map, and adds graphics stuff.
 */
class GraphicalMap : public Map {
private:
	GameWindow* 			m_window;
	std::list<MapObject>		m_objects;

public:
	explicit GraphicalMap(GameWindow* window);
	virtual ~GraphicalMap();

	const std::list<MapObject>&	get_objects() const { return m_objects; }
	virtual void			clear(); // Remove all objects

	virtual void			add_object(PacketReader& data);
};

#endif
