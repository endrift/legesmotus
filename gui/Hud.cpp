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
const Color Hud::BLUE_BLIP(0xAB, 0xC3, 0xFB);

const Color Hud::RED_BRIGHT(0xFF, 0xFA, 0xFA);
const Color Hud::RED_SHADOW(0x42, 0x55, 0x5E);
const Color Hud::RED_DARK(0xB5, 0x8B, 0x79);
const Color Hud::RED_BLIP(0xFB, 0xC3, 0xAB);

const Color Hud::DISABLED(0.4f, 0.4f, 0.4f);

const int Hud::SHADOW_CONVOLVE_WIDTH = 5;
const int Hud::SHADOW_CONVOLVE_HEIGHT = 5;
const float Hud::SHADOW_CONVOLVE_DATA[] = {
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

const float Hud::EDGE_SLOPE = 0.2f;
const float Hud::STROKE_WIDTH = 0.007f;

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
		case COLOR_BLIP:
			return BLUE_BLIP;
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
		case COLOR_BLIP:
			return RED_BLIP;
		}
		break;
	}
	return Color::BLACK;
}

Hud::Hud(ResourceCache* cache, Widget* parent) : Widget(parent), m_shadow_kernel(SHADOW_CONVOLVE_DATA, SHADOW_CONVOLVE_WIDTH, SHADOW_CONVOLVE_HEIGHT, 1) {
	m_cache = cache;

	m_main_font = NULL;

	m_active_player = NULL;

	m_player_status = new Widget(this);
	m_health = new ProgressBar(m_player_status);
	m_weapon = new ProgressBar(m_player_status);

	m_health_label = NULL;
	m_weapon_label = NULL;

	m_our_gate = new ProgressBar(this);
	m_their_gate = new ProgressBar(this);

	m_our_gate->set_orientation(true, false);

	reset_radar();
}

Hud::~Hud() {
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

	m_health->set_width(m_scale*0.18f);
	m_health->set_height(m_scale*0.03f);
	m_health->set_x(m_scale*0.11f);
	m_health->set_y(m_scale*0.03f);

	m_weapon->set_width(m_scale*0.18f);
	m_weapon->set_height(m_scale*0.03f);
	m_weapon->set_x(m_scale*0.12f);
	m_weapon->set_y(m_scale*0.09f);

	m_our_gate->set_width(m_scale*0.15f);
	m_our_gate->set_height(m_scale*0.04f);
	m_our_gate->set_x(-m_scale*0.13f);
	m_our_gate->set_cap_size(0.2f, 0.3f);

	m_their_gate->set_width(m_scale*0.15f);
	m_their_gate->set_height(m_scale*0.04f);
	m_their_gate->set_x(-m_scale*0.02f);
	m_their_gate->set_cap_size(0.2f, 0.3f);

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
	ctx->set_draw_color(get_team_color(m_active_team, COLOR_SHADOW));
}

void Hud::set_fg_active(DrawContext* ctx) const {
	ctx->set_blend_mode(DrawContext::BLEND_SCREEN);
	ctx->set_draw_color(get_team_color(m_active_team, COLOR_BRIGHT));
}

void Hud::draw_player_status(DrawContext* ctx) const {
	float points[8];

	points[0] = -m_scale*0.01f;
	points[1] = get_height() - m_scale*0.2f;

	points[2] = m_scale*(0.35f - 0.21f*EDGE_SLOPE);
	points[3] = get_height() - m_scale*0.2f;

	points[4] = m_scale*0.35f;
	points[5] = get_height() + m_scale*0.01f;

	points[6] = -m_scale*0.01f;
	points[7] = get_height() + m_scale*0.01f;

	set_bg_active(ctx);
	ctx->draw_polygon_fill(points, 4);

	set_fg_active(ctx);
	ctx->draw_stroke(points, 4, 0, m_scale*STROKE_WIDTH, true);

	m_player_status->draw(ctx);
}

void Hud::draw_game_status(DrawContext* ctx) const {
	float points[8];

	points[0] = -m_scale*0.16f;
	points[1] = m_scale*0.01f;

	points[2] = -m_scale*(0.16f - 0.17f*EDGE_SLOPE);
	points[3] = -m_scale*0.16f;

	points[4] = m_scale*(0.16f - 0.17f*EDGE_SLOPE);
	points[5] = -m_scale*0.16f;

	points[6] = m_scale*0.16f;
	points[7] = m_scale*0.01f;

	ctx->translate(get_width()*0.5f, get_height());

	set_bg_active(ctx);
	ctx->draw_polygon_fill(points, 4);

	set_fg_active(ctx);
	ctx->draw_stroke(points, 4, 0, m_scale*STROKE_WIDTH, true);

	// Draw gates (with clipping)
	ctx->translate(0.0f, -m_scale*0.09f);

	ctx->translate(m_scale*0.006f, m_scale*0.02f);
	ctx->start_clip();
	ctx->draw_rect_fill(m_scale*0.028f, m_scale*0.04f);
	ctx->finish_clip();
	ctx->translate(-m_scale*0.006f, -m_scale*0.02f);

	m_our_gate->draw(ctx);

	ctx->translate(-m_scale*0.006f, m_scale*0.02f);
	ctx->start_clip();
	ctx->clip_sub();
	ctx->draw_rect_fill(m_scale*0.08f, m_scale*0.04f);
	ctx->clip_add();
	ctx->draw_rect_fill(m_scale*0.028f, m_scale*0.04f);
	ctx->finish_clip();
	ctx->translate(m_scale*0.006f, -m_scale*0.02f);

	m_their_gate->draw(ctx);
	
	ctx->translate(0.0f, m_scale*0.02f);
	ctx->start_clip();
	ctx->clip_sub();
	ctx->draw_rect_fill(m_scale*0.05f, m_scale*0.04f);
	ctx->finish_clip();

	set_fg_active(ctx);
	ctx->draw_rect_fill(m_scale*0.004f, m_scale*0.04f);
	ctx->translate(0.0f, -m_scale*0.02f);

	ctx->translate(-get_width()*0.5f, m_scale*0.09f - get_height());
}

void Hud::draw_radar(DrawContext* ctx) const {
	ctx->push_transform();
	ctx->translate(get_width() - m_scale*0.15f, get_height() - m_scale*0.15f);

	set_bg_active(ctx);
	ctx->draw_arc_fill(1.0f, m_scale*0.17f, m_scale*0.17f, 32);

	// Clip off draw area
	ctx->start_clip();
	ctx->draw_arc(1.0f, m_scale*0.15f, m_scale*0.15f, 32);
	ctx->finish_clip();

	ctx->invert_clip();

	// Draw blips
	ctx->set_secondary_color(Color::BLACK);
	ctx->set_blend_mode(DrawContext::BLEND_SCREEN);

	ctx->translate(-m_radar_center.x*m_radar_scale, -m_radar_center.y*m_radar_scale);
	for (list<RadarBlip>::const_iterator blips = m_radar.begin(); blips != m_radar.end(); ++blips) {
		Color c = get_team_color(blips->team, COLOR_BLIP);

		switch (m_radar_mode) {
		case RADAR_ON:
			if (blips->frozen) {
				c.a = 0.5f;
			}
			break;

		case RADAR_AURAL:
			STUB(Hud::draw_radar);
			break;

		default:
			break;
		}

		ctx->set_draw_color(c);
		ctx->translate(blips->loc.x*m_radar_scale, blips->loc.y*m_radar_scale);
		ctx->draw_arc_fill(1.0f, m_scale*0.004f, m_scale*0.004f, 16);
		ctx->use_secondary_color(true);
		ctx->draw_ring_fill(1.0f, m_scale*0.01f, m_scale*0.004f, 16);
		ctx->use_secondary_color(false);
		ctx->translate(-blips->loc.x*m_radar_scale, -blips->loc.y*m_radar_scale);
	}
	ctx->translate(m_radar_center.x*m_radar_scale, m_radar_center.y*m_radar_scale);

	ctx->invert_clip();

	// Clean up clip
	ctx->start_clip();
	ctx->clip_sub();
	ctx->draw_rect_fill(m_scale*0.30f, m_scale*0.30f);
	ctx->finish_clip();

	set_fg_active(ctx);
	ctx->draw_ring_fill(1.0f, m_scale*(0.17f - STROKE_WIDTH), m_scale*0.17f, 32);

	ctx->set_secondary_color(get_team_color(m_active_team, COLOR_BRIGHT));
	ctx->set_draw_color(Color::BLACK);
	ctx->use_secondary_color(true);

	ctx->draw_ring_fill(1.0f, m_scale*0.15f, m_scale*0.1f, 32);
	ctx->use_secondary_color(false);

	ctx->pop_transform();
}

void Hud::update_radar(ConstIterator<pair<uint32_t, Player*> > players) {
	list<RadarBlip>::iterator blips = m_radar.begin();
	if (!players.has_more()) {
		m_radar.clear();
	}

	while (players.has_more()) {
		pair<uint32_t, Player*> p = players.next();

		while (blips != m_radar.end() && blips->id > p.second->get_id()) {
			// We passed by one...
			blips = m_radar.erase(blips);
		}

		if (blips == m_radar.end() || blips->id < p.second->get_id()) {
			RadarBlip blip = make_blip(p.second);
			m_radar.insert(blips, blip);
		} else if (blips->id == p.second->get_id()) {
			*blips = make_blip(p.second);
			++blips;
		}
	}

	while (blips != m_radar.end()) {
		blips = m_radar.erase(blips);
	}
}

Hud::RadarBlip Hud::make_blip(const Player* player) {
	RadarBlip blip;
	blip.id = player->get_id();
	blip.team = player->get_team();
	blip.frozen = player->is_frozen();
	blip.end_time = 0;
	blip.loc = player->get_position();
	return blip;
}

void Hud::set_player(GraphicalPlayer* player) {
	m_active_player = player;
}

void Hud::set_team(char team) {
	m_active_team = team;

	m_health->set_color(get_team_color(m_active_team, COLOR_BRIGHT), COLOR_SECONDARY);
	m_health_label->set_color(get_team_color(m_active_team, COLOR_BRIGHT));

	m_weapon->set_color(get_team_color(m_active_team, COLOR_BRIGHT), COLOR_PRIMARY);
	m_weapon->set_color(get_team_color(m_active_team, COLOR_BRIGHT), COLOR_SECONDARY);
	m_weapon_label->set_color(get_team_color(m_active_team, COLOR_BRIGHT));

	m_our_gate->set_color(get_team_color(m_active_team, COLOR_BLIP), COLOR_PRIMARY);
	m_our_gate->set_color(get_team_color(m_active_team, COLOR_BLIP), COLOR_SECONDARY);

	m_their_gate->set_color(get_team_color(m_active_team == 'A' ? 'B' : 'A', COLOR_BLIP), COLOR_PRIMARY);
	m_their_gate->set_color(get_team_color(m_active_team == 'A' ? 'B' : 'A', COLOR_BLIP), COLOR_SECONDARY);
}

void Hud::reset_radar() {
	m_radar_mode = RADAR_ON;
	m_radar_scale = 0.1;
	m_radar_blip_duration = 1000;
	m_radar.clear();
}

void Hud::set_radar_mode(RadarMode mode) {
	//m_radar_mode = mode;
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
	m_game_exists = logic != NULL;
	if (!m_game_exists) {
		m_active_player = NULL;
		return;
	}

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

		m_radar_center = m_active_player->get_position();
	}

	update_radar(logic->list_players());

	m_our_gate->set_progress(1.0f - logic->get_gate_progress(m_active_team));
	m_their_gate->set_progress(1.0f - logic->get_gate_progress(m_active_team == 'A' ? 'B' : 'A'));
}

void Hud::draw(DrawContext* ctx) const {
	if (m_game_exists) {
		if (m_active_player != NULL) {
			draw_player_status(ctx);
		}

		draw_game_status(ctx);

		if (m_radar_mode != RADAR_OFF) {
			draw_radar(ctx);
		}
	}
}
