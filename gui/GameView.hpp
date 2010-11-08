/*
 * gui/GameView.hpp
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

#ifndef LM_GUI_GAMEVIEW_HPP
#define LM_GUI_GAMEVIEW_HPP

#include "Widget.hpp"

namespace LM {
	class GameView : public Widget {
	private:
		float m_scale;
		float m_offset_x;
		float m_offset_y;
		float m_scale_base;

		void recalc_scale();

	public:
		GameView(Widget* parent = NULL);

		enum Layer {
			BACKGROUND = -1,
			PLAYERS = 0,
			FOREGROUND = 1,
			OVERLAY = 2,
		};

		virtual void set_width(float w);
		virtual void set_height(float h);
		void set_offset_x(float x);
		void set_offset_y(float y);
		void set_scale_base(float s);

		float get_scale() const;

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
