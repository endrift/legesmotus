/*
 * client/MapObject.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_MAPOBJECT_HPP
#define LM_CLIENT_MAPOBJECT_HPP

#include "common/Point.hpp"
#include "common/Polygon.hpp"
#include "client/Sprite.hpp"

/* Represents an object on the map */
class MapObject {
private:
	int			m_type;			// As defined in the enum in the Map class.
	Sprite*			m_sprite;
	Point			m_upper_left;
	Polygon			m_bounding_polygon;

public:
	MapObject();
	MapObject(int type, Sprite* sprite, Point upper_left);

	int		get_type() const { return m_type; }
	Sprite*		get_sprite() const { return m_sprite; }
	Point		get_upper_left() const { return m_upper_left; }
	Polygon&	get_bounding_polygon() { return m_bounding_polygon; }
	const Polygon&	get_bounding_polygon() const { return m_bounding_polygon; }

};

#endif
