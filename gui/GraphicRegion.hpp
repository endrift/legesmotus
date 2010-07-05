/*
 * gui/GraphicRegion.hpp
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

#ifndef LM_GUI_GRAPHICREGION_HPP
#define LM_GUI_GRAPHICREGION_HPP

#include "Graphic.hpp"

namespace LM {
	class GraphicRegion : public Graphic {
	private:
		int m_width;
		int m_height;

		float m_img_x;
		float m_img_y;
		float m_img_width;
		float m_img_height;

		bool m_repeat;

	public:
		explicit GraphicRegion(Image* image);
		GraphicRegion(const GraphicRegion& other);
		virtual GraphicRegion* clone() const;

		void set_width(int w);
		void set_height(int h);

		int get_width() const;
		int get_height() const;

		void set_image_x(float x);
		void set_image_y(float y);
		void set_image_width(float w);
		void set_image_height(float h);

		float get_image_x() const;
		float get_image_y() const;
		float get_image_width() const;
		float get_image_height() const;

		void set_image_repeat(bool repeat);

		virtual void draw(DrawContext* ctx) const;
	};
}

#endif
