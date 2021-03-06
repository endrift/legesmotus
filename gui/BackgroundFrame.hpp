/*
 * gui/BackgroundFrame.hpp
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

#ifndef LM_GUI_BACKGROUNDFRAME_HPP
#define LM_GUI_BACKGROUNDFRAME_HPP

#include "Widget.hpp"

namespace LM {
	class BackgroundFrame : public Widget {
	private:
		float m_border_radius;
		float m_border_padding;
		int m_max_corner_sections;
		bool m_use_thin_border;
		
		virtual void setup_clip(DrawContext* ctx) const;
		virtual void reset_clip(DrawContext* ctx) const;

	public:
		BackgroundFrame(Widget* parent = NULL);
	
		void set_border_radius(float radius);
		float get_border_radius() const;
		
		void set_border_padding(float padding);
		float get_border_padding() const;
		
		void set_border_max_roundness(int sections);
		
		void set_thin_border(bool thin);
		
		float get_internal_width() const;
		float get_internal_height() const;
	
		virtual void draw(DrawContext* ctx) const;
		virtual void draw_internals(DrawContext* ctx) const;
		
		virtual void draw_extras(DrawContext* ctx) const;
		
	};
}

#endif
