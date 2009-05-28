/*
 * client/Minimap.cpp
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

#include "Minimap.hpp"
#include "GameWindow.hpp"
#include <string>
#include <sstream>

using namespace std;

Minimap::Minimap(PathManager& path_manager, double scale) : m_path_manager(path_manager) {
	m_whole = new GraphicGroup;

	Sprite mask_sprite(m_path_manager.data_path("mini_circle.png", "sprites"));
	GraphicGroup group;
	Mask mask(&mask_sprite,&group);
	Sprite background(m_path_manager.data_path("mini_bg.png", "sprites"));

	mask.set_priority(Graphic::FOREGROUND);
	background.set_priority(Graphic::BACKGROUND);
	m_whole->add_graphic(&mask,"mask");
	m_whole->add_graphic(&background);

	m_minimask = static_cast<Mask*>(m_whole->get_graphic("mask"));
	m_minigroup = static_cast<GraphicGroup*>(m_minimask->get_masked());

	m_master_blip_red = new Sprite(m_path_manager.data_path("mini_blip_red.png", "sprites"));
	m_master_blip_blue = new Sprite(m_path_manager.data_path("mini_blip_blue.png", "sprites"));

	m_scale = scale;
}

Minimap::~Minimap() {
	delete m_whole;
	delete m_master_blip_red;
	delete m_master_blip_blue;
}

void Minimap::set_x(double x) {
	m_whole->set_x(x);
}

void Minimap::set_y(double y) {
	m_whole->set_y(y);
}

void Minimap::set_invisible(bool enable) {
	m_whole->set_invisible(enable);
}

void Minimap::add_blip(uint32_t id, char team, double x, double y) {
	Sprite *clone;
	if (team == 'A') {
		clone = m_master_blip_blue->clone();
	} else {
		clone = m_master_blip_red->clone();
	}
	clone->set_x(x*m_scale);
	clone->set_y(y*m_scale);
	stringstream s; // TODO move this to a private method?
	s << id; // Why is STL so bad?
	m_minigroup->add_graphic(clone,s.str());
	delete clone;
}

void Minimap::move_blip(uint32_t id, double x, double y) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_x(x*m_scale);
		blip->set_y(y*m_scale);
	}
}

void Minimap::set_blip_invisible(uint32_t id, bool invisible) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_invisible(invisible);
	}
}

void Minimap::remove_blip(uint32_t id) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	m_minigroup->remove_graphic(s.str());
	delete blip;
}

void Minimap::recenter(double x, double y) {
	m_minigroup->set_center_x(x*m_scale);
	m_minigroup->set_center_y(y*m_scale);
}

void Minimap::register_with_window(GameWindow* window) {
	window->register_hud_graphic(m_whole);
}

void Minimap::unregister_with_window(GameWindow* window) {
	window->unregister_hud_graphic(m_whole);
}
