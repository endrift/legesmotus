/*
 * gui/GuiClient.hpp
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

#ifndef LM_GUI_GUICLIENT_HPP
#define LM_GUI_GUICLIENT_HPP

#include "common/misc.hpp"
#include "newclient/Client.hpp"
#include "GraphicalPlayer.hpp"
#include "GraphicalMap.hpp"
#include "GraphicalWeapon.hpp"
#include "GameView.hpp"
#include "InputSink.hpp"
#include "PhysicsDraw.hpp"

namespace LM {
	class Window;
	class HumanController;
	class Player;
	class ResourceCache;
	class InputDriver;
	class Font;
	class Label;
	class ConvolveKernel;
	class Hud;

	class GuiClient : public Client, public InputSink {
	private:
		enum FontUse {
			FONT_BADGE = 0,
			FONT_BADGE_SHADOW,

			FONT_MAX
		};

		Window* m_window;
		HumanController* m_gcontrol;
		InputSink* m_input_sink;
		InputDriver* m_input;
		GameView* m_view;
		Widget m_root;
		PhysicsDraw *m_debugdraw;

		GraphicalPlayer* m_player;
		GraphicalMap* m_map;

		ResourceCache* m_cache;
		std::vector<std::string> m_preloaded_images;
		std::vector<std::string> m_preloaded_fonts;

		Hud* m_hud;

		std::map<int, Label*> m_badges;

		// XXX how much of this should be moved to a new class?
		Font* m_fonts[FONT_MAX];

		void preload();
		void preload_image(const char* filename);
		std::string preload_font(const char* filename, int size, const ConvolveKernel* kernel = NULL);

		void cleanup();

		void set_font(Font* font, FontUse fontuse);
		Font* get_font(FontUse font);
		Font* load_font(const char* filename, int size, const ConvolveKernel* kernel = NULL);

		void read_input();
		void set_sink(InputSink* input_sink);

		void add_badge(Player* player);
		void remove_badge(Player* player);
		void realign_badges();

	protected:
		virtual void add_player(Player* player);
		virtual void set_own_player(uint32_t id);
		virtual void remove_player(uint32_t id, const std::string& reason);

		virtual void set_map(Map* map);

	public:
		GuiClient();
		virtual ~GuiClient();

		virtual GraphicalPlayer* make_player(const char* name, uint32_t id, char team);
		virtual GraphicalMap* make_map();
		virtual Weapon* make_weapon(WeaponReader& weapon_data);

		virtual void name_change(Player* player, const std::string& new_name);
		virtual void team_change(Player* player, char new_team);

		virtual void run();
		void update_gui();

		virtual void key_pressed(const KeyEvent& event);
		virtual void mouse_moved(const MouseMotionEvent& event);
		virtual void mouse_clicked(const MouseButtonEvent& event);
		virtual void system_event(const SystemEvent& event);
		
		virtual void disconnect();
	};
}

#endif
