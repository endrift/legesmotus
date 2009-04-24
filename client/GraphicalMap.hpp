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
	MapObject*			m_gates[2];

	enum {
		GATE_HEIGHT = 109,	// Height of the gate bar
		GATE_EXTENT = 24	// Extent to which the gate's bounding polygon should extend outwards horizontally from the actual sprite
	}; // XXX: Don't hard code

	MapObject*			get_gate_object(char team);

public:
	explicit GraphicalMap(GameWindow* window);
	virtual ~GraphicalMap();

	const std::list<MapObject>&	get_objects() const { return m_objects; }
	virtual void			clear(); // Remove all objects

	virtual void			set_visible(bool visible);
	virtual void			add_object(PacketReader& data);

	// progress is in [0.0,1.0], where 0 == not lowered .. 1 == lowered all the way
	void				set_gate_progress(char team, double progress);
};

#endif
