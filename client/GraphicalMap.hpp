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

using namespace std;

/*
 * Derives from Map, and adds graphics stuff.
 */
class GraphicalMap : public Map {
private:
	list<MapObject>		m_objects;

public:
	const list<MapObject>&	get_objects() const { return m_objects; }

	virtual void		add_object(PacketReader& data);
};

#endif
