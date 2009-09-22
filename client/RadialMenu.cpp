/*
 * client/RadialMenu.cpp
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

#include "RadialMenu.hpp"
#include "common/math.hpp"
#include <iostream>

using namespace LM;
using namespace std;

RadialMenu::RadialMenu(RadialBackground* background, Color normal, Color hover) {
	m_background = background;
	m_normal = normal;
	m_hover = hover;
	m_active = 0;
	m_background->set_num_segments(1);
	m_background->set_segment_color(0, m_normal);
	m_background->set_priority(1);
	get_graphic_group()->give_graphic(m_background, "background");
}

int RadialMenu::coord_to_item(int x, int y) const {
	int x0 = x - m_background->get_x();
	int y0 = -y + m_background->get_y();
	double angle = -(atan2(double(y0), double(x0)) + (DEGREES_TO_RADIANS*m_background->get_rotation()));
	angle = fmod(angle + 4.0*M_PI + (3.0*M_PI/m_background->get_num_segments()), 2.0*M_PI);
	int i = int((angle + M_PI)/(2.0*M_PI) * m_background->get_num_segments()) % m_background->get_num_segments();
	cout << angle << " " << i << endl;
	return i;
}

void RadialMenu::recalc_segments() {
	double dist = m_background->get_inner_radius() +
		(m_background->get_outer_radius() - m_background->get_inner_radius())/2.0;
	double roffset = DEGREES_TO_RADIANS*m_background->get_rotation() + (M_PI/m_background->get_num_segments());
	int total = m_menu_items.size();
	m_background->set_num_segments(total);
	for (int i = 0; i < total; ++i) {
		m_background->set_segment_color(i, m_normal);
		Graphic* g = m_menu_items.at(i)->get_graphic();
		g->set_x(dist*cos(i*2.0*M_PI/total + roffset) + m_background->get_x());
		g->set_y(dist*sin(i*2.0*M_PI/total + roffset) + m_background->get_y());
	}
}

void RadialMenu::mouseover(MenuItem* item, int x, int y) {
	m_active = coord_to_item(x, y);
	m_background->set_segment_color(m_active, m_hover);
}

void RadialMenu::mouseout(MenuItem* item, int x, int y) {
	m_background->set_segment_color(m_active, m_normal);
}

void RadialMenu::add_item(MenuItem* item) {
	add_item_internal(item);
	recalc_segments();
}

void RadialMenu::remove_item(MenuItem* item) {
	remove_item_internal(item);
	recalc_segments();
}

MenuItem* RadialMenu::item_at_position(int x, int y) {
	int x0 = x - m_background->get_x();
	int y0 = y - m_background->get_y();
	if (sqrt(double(x0*x0) + double(y0*y0)) < m_background->get_inner_radius()) {
		return NULL;
	}
	return m_menu_items.at(coord_to_item(x, y));
}
