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
	Point			m_upper_left;
	Sprite*			m_sprite;
	char			m_team;			// 'A', 'B', or 0
	Polygon			m_bounding_polygon;	// Note: coordinates in this polygon are RELATIVE to m_upper_left

public:
	MapObject(int type, Point upper_left);

	int		get_type() const { return m_type; }
	Point		get_upper_left() const { return m_upper_left; }

	Sprite*		get_sprite() const { return m_sprite; }
	void		set_sprite(Sprite* sprite);
	bool		has_sprite() const { return m_sprite != NULL; }

	char		get_team() const { return m_team; }
	void		set_team(char team) { m_team = team; }

	Polygon&	get_bounding_polygon() { return m_bounding_polygon; }
	const Polygon&	get_bounding_polygon() const { return m_bounding_polygon; }

};

#endif