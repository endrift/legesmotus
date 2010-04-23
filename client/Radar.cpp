/*
 * client/Radar.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "Radar.hpp"
#include "GameWindow.hpp"
#include <string>
#include <sstream>

using namespace LM;
using namespace std;

Radar::Radar(PathManager& path_manager, double scale, RadarMode mode) : m_path_manager(path_manager) {
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
	m_mode = mode;
}

Radar::~Radar() {
	delete m_whole;
	delete m_master_blip_red;
	delete m_master_blip_blue;
}

void Radar::set_x(double x) {
	m_whole->set_x(x);
}

void Radar::set_y(double y) {
	m_whole->set_y(y);
}

void Radar::set_invisible(bool enable) {
	m_whole->set_invisible(m_mode == RADAR_OFF || enable);
}

void Radar::set_scale(double scale) {
	m_scale = scale;
}

void Radar::set_mode(RadarMode mode) {
	m_mode = mode;
	if (mode == RADAR_OFF) {
		set_invisible(true);
	} else {
		set_invisible(false);
	}
}

RadarMode Radar::get_mode() const {
	return m_mode;
}

void Radar::add_blip(uint32_t id, char team, double x, double y) {
	Sprite* clone;
	if (team == 'A') {
		clone = m_master_blip_blue->clone();
	} else {
		clone = m_master_blip_red->clone();
	}
	if (m_mode == RADAR_AURAL) {
		clone->set_alpha(0.0);
	}
	clone->set_x(x*m_scale);
	clone->set_y(y*m_scale);
	stringstream s; // TODO move this to a private method?
	s << id; // Why is STL so bad?
	m_minigroup->add_graphic(clone,s.str());
	delete clone;
}

void Radar::move_blip(uint32_t id, double x, double y) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_x(x*m_scale);
		blip->set_y(y*m_scale);
	}
}

void Radar::set_blip_invisible(uint32_t id, bool invisible) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_invisible(invisible);
	}
}

void Radar::set_blip_alpha(uint32_t id, double alpha) {
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		blip->set_alpha(alpha);
	}
}

void Radar::activate_blip(uint32_t id, uint64_t current, uint64_t duration) {
	if (m_mode != RADAR_AURAL) {
		return;
	}
	Graphic *blip;
	stringstream s;
	s << id;
	blip = m_minigroup->get_graphic(s.str());
	if (blip != NULL) {
		Transition* old = m_transman.get_transition(s.str());
		if (old != NULL) {
			m_transman.remove_transition(old); // Transition is deleted by the manager
		}
		m_transman.add_transition(new Transition(blip, &Graphic::set_alpha,
			new LinearCurve(1.0, 0.0), current, duration), s.str(), false, TransitionManager::DELETE);
		
	}
}

void Radar::remove_blip(uint32_t id) {
	stringstream s;
	s << id;
	m_minigroup->remove_graphic(s.str());
}

void Radar::recenter(double x, double y) {
	m_minigroup->set_center_x(x*m_scale);
	m_minigroup->set_center_y(y*m_scale);
}

void Radar::update(uint64_t tick) {
	m_transman.update(tick);
}

void Radar::register_with_window(GameWindow* window) {
	window->register_hud_graphic(m_whole);
}

void Radar::unregister_with_window(GameWindow* window) {
	window->unregister_hud_graphic(m_whole);
}
