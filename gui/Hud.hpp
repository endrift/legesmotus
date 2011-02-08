/*
 * gui/Hud.hpp
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

#ifndef LM_GUI_HUD_HPP
#define LM_GUI_HUD_HPP

#include "Widget.hpp"
#include "common/misc.hpp"
#include "ConvolveKernel.hpp"
#include "common/GameLogic.hpp"

#include <list>

namespace LM {
	class GraphicalPlayer;
	class ProgressBar;
	class Label;
	class Font;
	class ResourceCache;

	class Hud : public Widget {
	public:
		static const Color BLUE_BRIGHT;
		static const Color BLUE_SHADOW;
		static const Color BLUE_DARK;
		static const Color BLUE_BLIP;

		static const Color RED_BRIGHT;
		static const Color RED_SHADOW;
		static const Color RED_DARK;
		static const Color RED_BLIP;

		static const Color DISABLED;

		enum ColorType {
			COLOR_BRIGHT,
			COLOR_SHADOW,
			COLOR_DARK,
			COLOR_BLIP
		};

		// XXX put this somewhere that ACTUALLY MAKES SENSE (in common somewhere)
		enum RadarMode {
			RADAR_OFF,
			RADAR_AURAL,	// only players who have fired recently are displayed
			RADAR_ON,

			RADAR_MAX
		};

		static const Color& get_team_color(char team, ColorType type);

	private:
		static const int SHADOW_CONVOLVE_WIDTH;
		static const int SHADOW_CONVOLVE_HEIGHT;
		static const float SHADOW_CONVOLVE_DATA[];

		static const float EDGE_SLOPE;
		static const float STROKE_WIDTH;

		struct RadarBlip {
			// Do not use Player* in here, in case it gets deleted
			uint32_t id;
			char team;
			bool frozen;
			uint64_t end_time;
			Point loc;
		};

		ResourceCache* m_cache;

		ConvolveKernel m_shadow_kernel;

		GraphicalPlayer* m_active_player;
		char m_active_team;

		float m_scale;
		bool m_game_exists;

		RadarMode m_radar_mode;
		float m_radar_scale;
		uint64_t m_radar_blip_duration;
		std::list<RadarBlip> m_radar;
		Point m_radar_center;

		Font* m_main_font;

		Widget* m_player_status;
		ProgressBar* m_health;
		Label* m_health_label;
		ProgressBar* m_weapon;
		Label* m_weapon_label;

		ProgressBar* m_our_gate;
		ProgressBar* m_their_gate;

		void calc_scale();

		void set_bg_active(DrawContext* ctx) const;
		void set_fg_active(DrawContext* ctx) const;

		void draw_player_status(DrawContext* ctx) const;
		void draw_game_status(DrawContext* ctx) const;
		void draw_radar(DrawContext* ctx) const;
		void update_radar(ConstIterator<std::pair<uint32_t, Player*> > players);
		RadarBlip make_blip(const Player* player);

	public:
		Hud(ResourceCache* cache, Widget* parent = NULL);
		~Hud();

		void set_player(GraphicalPlayer* player);
		void set_team(char team);

		void reset_radar();
		void set_radar_mode(RadarMode mode);
		void set_radar_scale(float scale);
		void set_radar_blip_duration(uint64_t duration);

		virtual void set_width(float width);
		virtual void set_height(float height);

		const ConvolveKernel* get_shadow_kernel() const;

		void update(const GameLogic* logic);

		void draw(DrawContext* ctx) const;
	};
}

#endif
