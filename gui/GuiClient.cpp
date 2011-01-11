/*
 * gui/GuiClient.cpp
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

#include "GuiClient.hpp"
#include "SDLInputDriver.hpp"
#include "SDLWindow.hpp"
#include "ResourceCache.hpp"
#include "GraphicalPlayer.hpp"
#include "HumanController.hpp"
#include "common/Weapon.hpp"
#include "common/timer.hpp"
#include "Window.hpp"
#include "Bone.hpp"
#include "common/math.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "Hud.hpp"

using namespace LM;
using namespace std;

GuiClient::GuiClient() {
	// TODO move elsewhere
	m_window = SDLWindow::get_instance(1280, 800, 24, Window::FLAG_VSYNC);
	m_cache = new ResourceCache(get_res_directory(), m_window->get_context());
	m_input = new SDLInputDriver;
	m_input->set_sink(this);
	m_gcontrol = new HumanController; // XXX we don't necessarily want one
	m_gcontrol->set_viewport_size(m_window->get_width(), m_window->get_height());
	set_sink(m_gcontrol);
	set_controller(m_gcontrol);

	memset(m_fonts, 0, sizeof(m_fonts));

	preload();

	m_window->set_root_widget(&m_root);
	m_view = new GameView("gv", m_cache, m_window->get_width(), m_window->get_height(), 128, &m_root);

	m_debugdraw = new PhysicsDraw;
	#ifdef LM_DEBUG
	m_view->add_child(m_debugdraw, GameView::OVERLAY);
	#endif

	m_map = NULL;
	m_player = NULL;
	m_hud = NULL;
}

GuiClient::~GuiClient() {
	for (vector<string>::iterator iter = m_preloaded_images.begin(); iter != m_preloaded_images.end(); ++iter) {
		m_cache->decrement<Image>(*iter);
	}

	for (vector<string>::iterator iter = m_preloaded_fonts.begin(); iter != m_preloaded_fonts.end(); ++iter) {
		m_cache->decrement<Font>(*iter);
	}

	delete m_cache;
	delete m_input;
	delete m_view;

	// Clean up associations to prevent deletion failures
	m_root.set_parent(NULL);

	// TODO destroy window
}

void GuiClient::preload() {
	preload_image("red_head.png");
	preload_image("red_torso.png");
	preload_image("red_frontarm.png");
	preload_image("red_backarm.png");
	preload_image("red_frontleg.png");
	preload_image("red_backleg.png");
	preload_image("blue_head.png");
	preload_image("blue_torso.png");
	preload_image("blue_frontarm.png");
	preload_image("blue_backarm.png");
	preload_image("blue_frontleg.png");
	preload_image("blue_backleg.png");
	preload_image("aim.png");

	// XXX un-hardcode
	set_font(load_font("DustHomeMedium.ttf", 12), FONT_BADGE);
}

void GuiClient::preload_image(const char* filename) {
	Image img(filename, m_cache, true);
	m_cache->increment<Image>(filename);
	m_preloaded_images.push_back(filename);
}

string GuiClient::preload_font(const char* filename, int size, const ConvolveKernel* kernel) {
	Font font(filename, size, m_cache, false, kernel);
	const string& name = font.get_id();
	m_cache->increment<Font>(name);
	m_preloaded_fonts.push_back(name);
	return name;
}

void GuiClient::cleanup() {
	set_map(NULL);

	for (int i = 0; i < FONT_MAX; ++i) {
		set_font(NULL, (FontUse)i);
	}

	for (map<int, Label*>::iterator iter = m_badges.begin(); iter != m_badges.end(); ++iter) {
		delete iter->second;
	}
	m_badges.clear();

	delete m_hud;
	m_hud = NULL;
}

void GuiClient::set_font(Font* font, FontUse fontuse) {
	Font* oldfont = m_fonts[fontuse];
	if (font != NULL) {
		string newid = font->get_id();
		m_cache->increment<Font>(newid);
	}
	m_fonts[fontuse] = font;
	if (oldfont != NULL) {
		m_cache->decrement<Font>(oldfont->get_id());
	}
}

Font* GuiClient::get_font(FontUse font) {
	return m_fonts[font];
}

Font* GuiClient::load_font(const char* filename, int size, const ConvolveKernel* kernel) {
	string id = Font::lookup_id(filename, size, false, kernel);
	Font* font = m_cache->get<Font>(id);
	if (font == NULL) {
		id = preload_font(filename, size, kernel);
	}
	return m_cache->get<Font>(id);
}

void GuiClient::set_sink(InputSink* input_sink) {
	m_input_sink = input_sink;
}

void GuiClient::add_badge(Player* player) {
	Label* badge = new Label(player->get_name(), get_font(FONT_BADGE), &m_root);
	Label* shadow = new Label(player->get_name(), get_font(FONT_BADGE_SHADOW));
	m_badges[player->get_id()] = badge;
	badge->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_DARK));
	badge->set_align(Label::ALIGN_CENTER);
	shadow->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_SHADOW));
	shadow->set_blend(DrawContext::BLEND_SUBTRACT);
	badge->set_shadow(shadow);
}

void GuiClient::remove_badge(Player* player) {
	map<int, Label*>::iterator iter = m_badges.find(player->get_id());
	delete iter->second;
	m_badges.erase(iter);
}

void GuiClient::realign_badges() {
	for (map<int, Label*>::iterator iter = m_badges.begin(); iter != m_badges.end(); ++iter) {
		Player* player = get_game()->get_player(iter->first);
		Point new_point = m_view->world_to_view(Point(player->get_x(), player->get_y()));
		iter->second->set_x(new_point.x);
		iter->second->set_y(new_point.y - 64.0f*m_view->get_scale());
	}
}

void GuiClient::add_player(Player* player) {
	Client::add_player(player);
	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(player);
	m_view->add_child(gp->get_graphic(), GameView::PLAYERS);
	add_badge(player);
}

void GuiClient::set_own_player(uint32_t id) {
	Client::set_own_player(id);
	m_player = static_cast<GraphicalPlayer*>(get_player(id));
}

void GuiClient::remove_player(uint32_t id, const std::string& reason) {
	Player *p = get_player(id);
	if (p == NULL) {
		WARN("Can't remove a non-existent player");
		return;
	}

	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(p);
	m_view->remove_child(gp->get_graphic());
	remove_badge(p);

	if (m_player != NULL && m_player->get_id() == id) {
		m_player = NULL;
	}

	Client::remove_player(id, reason);
}

void GuiClient::set_map(Map* map) {
	Client::set_map(map);
	if (m_map != NULL) {
		m_view->remove_child(m_map->get_background());
		m_debugdraw->set_world(NULL);
	}
	if (map != NULL) {
		m_map = static_cast<GraphicalMap*>(map);
		m_view->add_child(m_map->get_background(), GameView::BACKGROUND);
		if (get_game() != NULL) {
			m_debugdraw->set_world(get_game()->get_world());
		}
	}
}

GraphicalPlayer* GuiClient::make_player(const char* name, uint32_t id, char team) {
	return new GraphicalPlayer(name, id, team, m_cache);
}

GraphicalMap* GuiClient::make_map() {
	return new GraphicalMap(m_cache);
}

Weapon* GuiClient::make_weapon(WeaponReader& weapon_data) {
	GraphicalWeapon* gw = new GraphicalWeapon(m_cache);
	Weapon* w = Weapon::new_weapon(weapon_data, gw);
	if (w == NULL) {
		delete gw;
	}
	return w;
}

void GuiClient::name_change(Player* player, const std::string& new_name) {
	Client::name_change(player, new_name);
	Label* badge = m_badges[player->get_id()];
	badge->set_string(new_name);
}

void GuiClient::team_change(Player* player, char new_team) {
	Client::team_change(player, new_team);
	Label* badge = m_badges[player->get_id()];
	Label* shadow = badge->get_shadow();
	badge->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_DARK));
	shadow->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_SHADOW));
}

void GuiClient::run() {
	INFO("Beginning running GuiClient...");
	set_running(true);
	// XXX testing code
	Bone crosshair_bone;
	GraphicContainer aim(false, &m_root);
	Sprite crosshair(m_cache->get<Image>("aim.png"));
	crosshair.set_center_x(128);
	crosshair.set_center_y(128);
	crosshair.set_x(512);
	crosshair.get_bone()->set_parent(&crosshair_bone);
	aim.add_graphic(&crosshair);
	aim.set_x(m_window->get_width()/2);
	aim.set_y(m_window->get_height()/2);
	m_view->set_scale_base(1024);
	crosshair_bone.set_scale_x(m_view->get_scale()/4.0f);
	crosshair_bone.set_scale_y(m_view->get_scale()/4.0f);
	m_hud = new Hud(&m_root);
	set_font(load_font("DustHomeMedium.ttf", 12, m_hud->get_shadow_kernel()), FONT_BADGE_SHADOW);

	IPAddress host;
	resolve_hostname(host, "endrift.com", 16876);
	connect(host);
	// XXX end testing code

	uint64_t last_time = get_ticks();
	while (running()) {
		uint64_t current_time = get_ticks();
		uint64_t diff = current_time - last_time;

		m_input->update();
		
		if (!running()) {
			break;
		}

		// Fudge the current time so that the remaining time between steps is accounted for
		current_time -= step(diff);

		// XXX move to client, game logic
		crosshair_bone.set_rotation(m_gcontrol->get_aim() * RADIANS_TO_DEGREES);
		// XXX end

		update_gui();

		m_window->redraw();
		last_time = current_time;
	}

	cleanup();
}

void GuiClient::update_gui() {
	// Recenter player
	if (m_player != NULL) {
		m_view->set_offset_x(m_player->get_x() * m_view->get_scale());
		m_view->set_offset_y(m_player->get_y() * m_view->get_scale());
	}

	// Move badges
	realign_badges();
}

void GuiClient::key_pressed(const KeyEvent& event) {
	m_input_sink->key_pressed(event);

	if (event.type == KEY_ESCAPE) {
		disconnect();
	}
}

void GuiClient::mouse_moved(const MouseMotionEvent& event) {
	m_input_sink->mouse_moved(event);
}

void GuiClient::mouse_clicked(const MouseButtonEvent& event) {
	m_input_sink->mouse_clicked(event);
}

void GuiClient::system_event(const SystemEvent& event) {
	m_input_sink->system_event(event);

	if (event.type == SYSTEM_QUIT) {
		disconnect();
	}
}

void GuiClient::disconnect() {
	cleanup();

	Client::disconnect();
	
	DEBUG("Disconnected.");
}
