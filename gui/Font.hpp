/*
 * gui/Font.hpp
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

#ifndef LM_GUI_FONT_HPP
#define LM_GUI_FONT_HPP

#include <map>
#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "DrawContext.hpp"

// A temporary measure
#define Font NewFont

namespace LM {
	class Font {
	public:
		struct Glyph {
		private:
			DrawContext* m_ctx;

		public:
			DrawContext::Image image;
			float advance;
			float bearing;
			float baseline;
			float width;
			float height;
			int bitmap_width;
			int bitmap_height;

			Glyph();
			Glyph(const FT_GlyphSlot& glyph, DrawContext* ctx);
			~Glyph();
		};

	private:
		static FT_Library m_library;
		static bool m_init;

		FT_Face m_face;
		std::map<int, Glyph*> m_glyphs;
		DrawContext* m_ctx;

	public:
		Font(const std::string& filename, float size, DrawContext* ctx);
		~Font();

		const Glyph* get_glyph(int character);
		float get_height() const;
		float kern(int lchar, int rchar) const;
	};
}

#endif
