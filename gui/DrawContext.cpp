/*
 * gui/DrawContext.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "DrawContext.hpp"
#include "Widget.hpp"

using namespace LM;
using namespace std;

DrawContext::DrawContext() {
	m_root_widget = NULL;
}

DrawContext::~DrawContext() {
	delete m_root_widget;
}

void DrawContext::set_root_widget(Widget* widget) {
	m_root_widget = widget;
	widget->set_parent(NULL);
}

Widget* DrawContext::get_root_widget() {
	return m_root_widget;
}

void DrawContext::draw_roundrect(float w, float h, float r, int fine) {
	draw_roundrect_fill(w, h, r, fine);
	draw_roundrect_line(w, h, r, fine);
}

void DrawContext::draw_roundrect_fill(float w, float h, float r, int fine) {
	if (w - 2.0f*r < 0) {
		r = w/2.0f;
	}
	if (h - 2.0f*r < 0) {
		r = h/2.0f;
	}

	float r_t2 = 2.0f*r;
	float r_o2 = 0.5f*r;

	// Draw center
	draw_rect_fill(w - r_t2, h - r_t2);

	// Draw sides
	translate(w/2.0f - r_o2, 0);
	draw_rect_fill(r, h - r_t2);
	translate(r - w, 0);
	draw_rect_fill(r, h - r_t2);
	translate(w/2.0f - r_o2, h/2.0f - r_o2);
	draw_rect_fill(w - r_t2, r);
	translate(0, r - h);
	draw_rect_fill(w - r_t2, r);

	// Recenter
	translate(0, h/2.0f - r_o2);

	// Draw corners
	translate(w/2.0f - r, h/2.0f - r);
	draw_arc_fill(0.25f, r, r, fine);

	translate(r_t2 - w, 0);
	rotate(90);
	draw_arc_fill(0.25f, r, r, fine);

	translate(r_t2 - h, 0);
	rotate(90);
	draw_arc_fill(0.25f, r, r, fine);

	translate(r_t2 - w, 0);
	rotate(90);
	draw_arc_fill(0.25f, r, r, fine);

	// Recenter
	translate(r - h/2.0f, r - w/2.0f);
	rotate(90);
}

void DrawContext::draw_roundrect_line(float w, float h, float r, int fine) {
	if (w - 2.0f*r < 0) {
		r = w/2.0f;
	}
	if (h - 2.0f*r < 0) {
		r = h/2.0f;
	}

	float r_t2 = 2.0f*r;

	// Draw edges
	draw_line(w/2.0f, h/2.0f - r, w/2.0f, r - h/2.0f);
	draw_line(-w/2.0f, h/2.0f - r, -w/2.0f, r - h/2.0f);
	draw_line(w/2.0f - r, h/2.0f, r - w/2.0f, h/2.0f);
	draw_line(w/2.0f - r, -h/2.0f, r - w/2.0f, -h/2.0f);

	// Draw corners
	translate(w/2.0f - r, h/2.0f - r);
	draw_arc_line(0.25f, r, r, fine);

	translate(r_t2 - w, 0);
	rotate(90);
	draw_arc_line(0.25f, r, r, fine);

	translate(r_t2 - h, 0);
	rotate(90);
	draw_arc_line(0.25f, r, r, fine);

	translate(r_t2 - w, 0);
	rotate(90);
	draw_arc_line(0.25f, r, r, fine);

	// Recenter
	translate(r - h/2.0f, r - w/2.0f);
	rotate(90);
}
