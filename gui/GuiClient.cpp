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
#include "common/timer.hpp"
#include "Window.hpp"

using namespace LM;
using namespace std;

GuiClient::GuiClient() {
	// TODO move elsewhere
	m_window = SDLWindow::get_instance(800, 600, 24, 0);
	m_cache = new ResourceCache(get_res_directory(), m_window->get_context());

	preload();
}

GuiClient::~GuiClient() {
	delete m_cache;

	// TODO destroy window
}

void GuiClient::preload() {
	Image red_head("red_head.png", m_cache, true);
	Image red_torso("red_torso.png", m_cache, true);
	Image red_farm("red_frontarm.png", m_cache, true);
	Image red_barm("red_backarm.png", m_cache, true);
	Image red_fleg("red_frontleg.png", m_cache, true);
	Image red_bleg("red_backleg.png", m_cache, true);

	Image blue_head("blue_head.png", m_cache, true);
	Image blue_torso("blue_torso.png", m_cache, true);
	Image blue_farm("blue_frontarm.png", m_cache, true);
	Image blue_barm("blue_backarm.png", m_cache, true);
	Image blue_fleg("blue_frontleg.png", m_cache, true);
	Image blue_bleg("blue_backleg.png", m_cache, true);

	// Hold onto the images
	m_cache->increment<Image>("red_head.png");
	m_cache->increment<Image>("red_torso.png");
	m_cache->increment<Image>("red_frontarm.png");
	m_cache->increment<Image>("red_backarm.png");
	m_cache->increment<Image>("red_frontleg.png");
	m_cache->increment<Image>("red_backleg.png");

	m_cache->increment<Image>("blue_head.png");
	m_cache->increment<Image>("blue_torso.png");
	m_cache->increment<Image>("blue_frontarm.png");
	m_cache->increment<Image>("blue_backarm.png");
	m_cache->increment<Image>("blue_frontleg.png");
	m_cache->increment<Image>("blue_backleg.png");
}

GraphicalPlayer* GuiClient::make_player(const char* name, uint32_t id, char team) {
	return new GraphicalPlayer(name, id, team, m_cache);
}

void GuiClient::run() {
	// XXX testing code
	GraphicalPlayer a_player("foo", 0, 'A', m_cache);
	m_window->set_root_widget(a_player.get_graphic());
	a_player.set_x(32);
	a_player.set_y(48);
	// XXX end testing code

	uint64_t last_time = get_ticks();
	while (true) {
		uint64_t current_time = get_ticks();
		step(current_time - last_time);
		m_window->redraw();
		last_time = current_time;
	}
}
