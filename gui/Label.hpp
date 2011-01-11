/*
 * gui/Label.hpp
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

#ifndef LM_GUI_LABEL_HPP
#define LM_GUI_LABEL_HPP

#include "Widget.hpp"
#include "Font.hpp"
#include "DrawContext.hpp"
#include "common/misc.hpp"
#include <string>

namespace LM {
	class Label : public Widget {
	public:
		enum Align {
			ALIGN_LEFT,
			ALIGN_RIGHT,
			ALIGN_CENTER
		};

		enum VAlign {
			VALIGN_TOP,
			VALIGN_BOTTOM,
			VALIGN_MIDDLE
		};

	private:
		Font* m_font;
		std::wstring m_text;
		Align m_align;
		Color m_color;
		DrawContext::BlendMode m_blend;
		float m_tracking;
		Label* m_shadow;
		float m_skew;
		VAlign m_skew_align;

		void recalculate_width();

	public:
		Label(Font* font = NULL, Widget* parent = NULL);
		Label(const std::wstring& str, Font* font, Widget* parent = NULL);
		Label(const std::string& str, Font* font, Widget* parent = NULL);
		~Label();

		void set_string(const std::wstring& str);
		void set_string(const std::string& str);

		void set_color(Color color);
		void set_blend(DrawContext::BlendMode mode);

		void set_align(Align align);
		Align get_align() const;

		void set_tracking(float tracking);

		void set_skew(float skew);
		void set_skew_align(VAlign align);
		VAlign get_skew_align() const;

		void set_shadow(Label* shadow);
		Label* get_shadow();

		void set_font(Font* font);
		const Font* get_font() const;

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
