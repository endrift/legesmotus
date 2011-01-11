/*
 * gui/Graphic.hpp
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

#ifndef LM_GUI_GRAPHIC_HPP
#define LM_GUI_GRAPHIC_HPP

#include "DrawContext.hpp"
#include "Image.hpp"
#include "Bone.hpp"
#include "common/misc.hpp"

#include <map>

namespace LM {
	class Graphic {
	private:
		Image	m_image;

		Bone	m_bone;

		int		m_image_w;
		int		m_image_h;

		bool	m_invisible;

		Color	m_color;

		ShaderSet* m_shaders;

	protected:
		Graphic(Image* image = NULL);
		Graphic(const Graphic& other);

		void	transform(DrawContext* ctx) const;
		void	preprocess(DrawContext* ctx) const;
		void	postprocess(DrawContext* ctx) const;

	public:
		virtual	~Graphic();
		virtual Graphic* clone() const = 0;

		void	set_x(float x);
		void	set_y(float y);
		void	set_scale_x(float scale_x);
		void	set_scale_y(float scale_y);
		void	set_rotation(float rotation);

		float	get_x() const;
		float	get_y() const;
		float	get_scale_x() const;
		float	get_scale_y() const;
		float	get_rotation() const;

		void	set_center_x(float center_x);
		void	set_center_y(float center_y);

		float	get_center_x() const;
		float	get_center_y() const;

		void	set_image(Image image);
		Image	get_image();
		const Image&	get_image() const;

		void	set_bone(const Bone& bone);
		Bone*	get_bone();

		void	set_invisible(bool invisible);
		bool	is_invisible() const;

		void	set_color(const Color& c);
		const Color& get_color() const;

		void	set_shader_set(ShaderSet* shaders);

		virtual void draw(DrawContext* ctx) const = 0;
	};
}

#endif
