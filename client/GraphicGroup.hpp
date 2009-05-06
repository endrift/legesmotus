/*
 * client/GraphicGroup.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHICGROUP_HPP
#define LM_CLIENT_GRAPHICGROUP_HPP

#include "Graphic.hpp"
#include <list>
#include <string>
#include <map>

class GraphicGroup : public Graphic {
private:
	std::list<Graphic*>	m_graphics;
	std::map<std::string, Graphic*> m_names;
public:
	GraphicGroup();
	GraphicGroup(const GraphicGroup& other);
	virtual ~GraphicGroup();
	virtual GraphicGroup* clone() const;

	// Adding a graphic to the group clones it
	void		add_graphic(Graphic* graphic); // If this method is used, the graphic is inaccessable outside of the object
	void		add_graphic(Graphic* graphic, const std::string& name);
	Graphic*	get_graphic(const std::string& name);
	void		remove_graphic(const std::string& name); // Removing the graphic doesn't delete it -- you must delete it yourself

	// These act recursively
	virtual void	set_alpha(double alpha);
	virtual void	set_red_intensity(double r);
	virtual void	set_green_intensity(double g);
	virtual void	set_blue_intensity(double b);

	void		draw(const GameWindow* window) const;
};

#endif
