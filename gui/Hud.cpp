/*
 * gui/Hud.cpp
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

#include "Hud.hpp"
#include "GraphicalPlayer.hpp"
#include "ProgressBar.hpp"
#include "Label.hpp"
#include "Font.hpp"
#include "ResourceCache.hpp"
#include "common/Weapon.hpp"

using namespace LM;
using namespace std;

const Color Hud::BLUE_BRIGHT(0xFA, 0xFA, 0xFF);
const Color Hud::BLUE_SHADOW(0x5E, 0x55, 0x42);
const Color Hud::BLUE_DARK(0x79, 0x8B, 0xB5);

const Color Hud::RED_BRIGHT(0xFF, 0xFA, 0xFA);
const Color Hud::RED_SHADOW(0x42, 0x55, 0x5E);
const Color Hud::RED_DARK(0xB5, 0x8B, 0x79);

const Color Hud::DISABLED(0.4f, 0.4f, 0.4f);

const int Hud::m_shadow_convolve_height = 5;
const int Hud::m_shadow_convolve_width = 5;
const float Hud::m_shadow_convolve_data[] = {
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

const Color& Hud::get_team_color(char team, ColorType type) {
	switch (team) {
	case 'A':
		switch (type) {
		case COLOR_BRIGHT:
			return BLUE_BRIGHT;
		case COLOR_SHADOW:
			return BLUE_SHADOW;
		case COLOR_DARK:
			return BLUE_DARK;
		}
		break;
	case 'B':
		switch (type) {
		case COLOR_BRIGHT:
			return RED_BRIGHT;
		case COLOR_SHADOW:
			return RED_SHADOW;
		case COLOR_DARK:
			return RED_DARK;
		}
		break;
	}
	return Color::BLACK;
}

Hud::Hud(ResourceCache* cache, Widget* parent) : Widget(parent), m_shadow_kernel(m_shadow_convolve_data, m_shadow_convolve_width, m_shadow_convolve_height, 1) {
	m_cache = cache;

	m_main_font = NULL;

	m_active_player = NULL;

	m_player_status = new Widget(this);
	m_health = new ProgressBar(m_player_status);
	m_weapon = new ProgressBar(m_player_status);

	m_health_label = NULL;
	m_weapon_label = NULL;

	reset_radar();
}

Hud::~Hud() {
	delete m_player_status;
	
	if (m_main_font != NULL) {
		m_cache->decrement<Font>(m_main_font->get_id());
	}
}

void Hud::calc_scale() {
	m_scale = min<float>(get_width(), get_height());

	if (m_scale == 0) {
		return;
	}

	m_player_status->set_y(get_height() - m_scale*0.2f);

	m_health->set_width(m_scale*0.2f);
	m_health->set_height(m_scale*0.03f);
	m_health->set_x(m_scale*0.11f);
	m_health->set_y(m_scale*0.03f);

	m_weapon->set_width(m_scale*0.2f);
	m_weapon->set_height(m_scale*0.03f);
	m_weapon->set_x(m_scale*0.12f);
	m_weapon->set_y(m_scale*0.09f);

	// XXX move font name
	delete m_health_label;
	delete m_weapon_label;

	if (m_main_font != NULL) {
		m_cache->decrement<Font>(m_main_font->get_id());
	}

	m_main_font = m_cache->load_font("DustHomeMedium.ttf", m_scale*0.022f);

	m_health_label = new Label("Energy", m_main_font, m_player_status);
	m_health_label->set_x(m_scale*0.02f);
	m_health_label->set_y(m_scale*0.052f);
	m_weapon_label = new Label("Weapon", m_main_font, m_player_status);
	m_weapon_label->set_x(m_scale*0.02f);
	m_weapon_label->set_y(m_scale*0.112f);
}

void Hud::set_bg_active(DrawContext* ctx) const {
	ctx->set_blend_mode(DrawContext::BLEND_SUBTRACT);
	ctx->set_draw_color(get_team_color(m_active_player->get_team(), COLOR_SHADOW));
}

void Hud::set_fg_active(DrawContext* ctx) const {
	ctx->set_blend_mode(DrawContext::BLEND_SCREEN);
	ctx->set_draw_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT));
}

void Hud::draw_player_status(DrawContext* ctx) const {
	float points[8];

	points[0] = 0.0f;
	points[1] = get_height();
	points[2] = m_scale*0.35;
	points[3] = get_height();
	points[4] = m_scale*0.31;
	points[5] = get_height() - m_scale*0.2f;
	points[6] = 0.0f;
	points[7] = get_height() - m_scale*0.2f;

	set_bg_active(ctx);
	ctx->draw_polygon_fill(points, 4);

	set_fg_active(ctx);
	ctx->draw_stroke(points, 4, 0, m_scale*0.007f, true);

	m_player_status->draw(ctx);
}

void Hud::set_player(GraphicalPlayer* player) {
	m_active_player = player;

	m_health->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT), COLOR_SECONDARY);
	m_health_label->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT));

	m_weapon->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT), COLOR_PRIMARY);
	m_weapon->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT), COLOR_SECONDARY);
	m_weapon_label->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT));
}

void Hud::reset_radar() {
	m_radar_mode = RADAR_ON;
	m_radar_scale = 0.1;
	m_radar_blip_duration = 1000;
}

void Hud::set_radar_mode(RadarMode mode) {
	m_radar_mode = mode;
}

void Hud::set_radar_scale(float scale) {
	m_radar_scale = scale;
}

void Hud::set_radar_blip_duration(uint64_t duration) {
	m_radar_blip_duration = 1000;
}

void Hud::set_width(float width) {
	Widget::set_width(width);
	calc_scale();
}

void Hud::set_height(float height) {
	Widget::set_height(height);
	calc_scale();
}

const ConvolveKernel* Hud::get_shadow_kernel() const {
	return &m_shadow_kernel;
}

void Hud::update(const GameLogic* logic) {
	if (m_active_player != NULL) {
		if (m_active_player->is_frozen()) {
			m_health->set_color(DISABLED, COLOR_PRIMARY);

			m_health->set_progress(1.0f - m_active_player->get_remaining_freeze()/(float)m_active_player->get_freeze_time());
		} else {
			m_health->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT), COLOR_PRIMARY);
	
			m_health->set_progress(m_active_player->get_energy()/(float)Player::MAX_ENERGY);
		}

		const Weapon* weapon = logic->get_weapon(m_active_player->get_current_weapon_id());

		if (false /* TODO magically get weapon switch delay from client??? */) {
			m_weapon->set_color(DISABLED, COLOR_PRIMARY);

			// TODO more magic
		} else {
			m_weapon->set_color(get_team_color(m_active_player->get_team(), COLOR_BRIGHT), COLOR_PRIMARY);

			m_weapon->set_progress(1.0f - weapon->get_remaining_cooldown()/(float)weapon->get_total_cooldown());
		}
	}
}

void Hud::draw(DrawContext* ctx) const {
	if (m_active_player != NULL) {
		draw_player_status(ctx);
	}
}
