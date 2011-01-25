/*
 * gui/ProgressBar.cpp
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

#include "ProgressBar.hpp"
#include "DrawContext.hpp"
#include <cmath>

using namespace LM;
using namespace std;


const float ProgressBar::CAP_WIDTH = 0.3f;
const float ProgressBar::CAP_SEPARATOR = 0.2f;
const float ProgressBar::SKEW = 0.2f;

ProgressBar::ProgressBar(Widget* parent) : Widget(parent) {
	m_progress = 0;
	m_vertical = false;
	m_flipped = false;
	m_skew = SKEW;
	m_cap = CAP_WIDTH;
	m_spacer = CAP_SEPARATOR;
}

void ProgressBar::draw_section(float size, DrawContext* ctx) const {
	float verts[8];
	int odd;
	float thick;
	float skew = m_flipped ? -m_skew : m_skew;
	if (m_vertical) {
		thick = get_width();
		odd = 0;
	} else {
		thick = get_height();
		odd = 1;
	}

	verts[odd | 0] = 0.0f;
	verts[odd | 2] = 0.0f;
	verts[odd | 4] = thick;
	verts[odd | 6] = thick;

	if (skew < 0) {
		verts[(odd ^ 1) | 0] = -thick*skew;
		verts[(odd ^ 1) | 2] = size - thick*skew;
		verts[(odd ^ 1) | 4] = size;
		verts[(odd ^ 1) | 6] = 0.0f;
	} else {
		verts[(odd ^ 1) | 0] = 0.0f;
		verts[(odd ^ 1) | 2] = size;
		verts[(odd ^ 1) | 4] = size + thick*skew;
		verts[(odd ^ 1) | 6] = thick*skew;
	}
	
	ctx->draw_polygon_fill(verts, 4);
}

void ProgressBar::set_progress(float progress) {
	m_progress = progress;
}

float ProgressBar::get_progress() const {
	return m_progress;
}

void ProgressBar::set_orientation(bool flip, bool vert) {
	m_vertical = vert;
	m_flipped = flip;
}

void ProgressBar::set_skew(float skew) {
	m_skew = skew;
}

void ProgressBar::set_cap_size(float cap, float spacer) {
	m_cap = cap;
	m_spacer = spacer;
}

void ProgressBar::draw(DrawContext* ctx) const {
	float thick = m_vertical?get_width():get_height();
	float skew_size = fabsf(m_skew) * thick;
	float progress_base = m_vertical?get_height():get_width();
	float cap_width = m_cap * thick;
	float cap_separator = m_spacer * thick;
	float progress_width = progress_base - skew_size - (cap_width + cap_separator)*2.0f;
	ctx->push_transform();
	ctx->translate(get_x(), get_y());
	if (progress_width >= skew_size) {
		ctx->set_draw_color(get_color(COLOR_SECONDARY));
		draw_section(cap_width, ctx);

		if (m_vertical) {
			ctx->translate(0, cap_width + cap_separator);
		} else {
			ctx->translate(cap_width + cap_separator, 0);
		}

		ctx->set_draw_color(get_color(COLOR_PRIMARY));
		if (m_flipped) {
			if (m_vertical) {
				ctx->translate(0, progress_width*(1.0f - m_progress));
			} else {
				ctx->translate(progress_width*(1.0f - m_progress), 0);
			}

			draw_section(progress_width*m_progress, ctx);
			
			if (m_vertical) {
				ctx->translate(0, progress_width*m_progress + cap_separator);
			} else {
				ctx->translate(progress_width*m_progress + cap_separator, 0);
			}
		} else {
			draw_section(progress_width*m_progress, ctx);

			if (m_vertical) {
				ctx->translate(0, progress_width + cap_separator);
			} else {
				ctx->translate(progress_width + cap_separator, 0);
			}
		}

		ctx->set_draw_color(get_color(COLOR_SECONDARY));
		draw_section(cap_width, ctx);
	} else {
		ctx->set_draw_color(get_color(COLOR_PRIMARY));
		draw_section(progress_base*m_progress, ctx);
	}
	ctx->pop_transform();
}
