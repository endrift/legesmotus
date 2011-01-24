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
#include "newclient/GameLogic.hpp"

namespace LM {
	class GraphicalPlayer;
	class ProgressBar;
	class Label;

	class Hud : public Widget {
	public:
		static const Color BLUE_BRIGHT;
		static const Color BLUE_SHADOW;
		static const Color BLUE_DARK;

		static const Color RED_BRIGHT;
		static const Color RED_SHADOW;
		static const Color RED_DARK;

		enum ColorType {
			COLOR_BRIGHT,
			COLOR_SHADOW,
			COLOR_DARK
		};

		static const Color& get_team_color(char team, ColorType type);

	private:
		static const int m_shadow_convolve_height;
		static const int m_shadow_convolve_width;
		static const float m_shadow_convolve_data[];

		ConvolveKernel m_shadow_kernel;

		GraphicalPlayer* m_active_player;

		float m_scale;

		Widget* m_player_status;
		ProgressBar* m_health;
		Label* m_health_label;
		ProgressBar* m_weapon;
		Label* m_weapon_label;

		void calc_scale();

		void set_bg_active(DrawContext* ctx) const;
		void set_fg_active(DrawContext* ctx) const;

		void draw_player_status(DrawContext* ctx) const;

	public:
		Hud(Widget* parent = NULL);

		void set_player(GraphicalPlayer* player);

		virtual void set_width(float width);
		virtual void set_height(float height);

		const ConvolveKernel* get_shadow_kernel() const;

		void update(const GameLogic* logic);

		void draw(DrawContext* ctx) const;
	};
}

#endif
