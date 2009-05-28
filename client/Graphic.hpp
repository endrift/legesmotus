/*
 * client/Graphic.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_CLIENT_GRAPHIC_HPP
#define LM_CLIENT_GRAPHIC_HPP

#include "compat_gl.h"
#include "SDL.h"
#include "common/misc.hpp"
#include <string>

class GameWindow;
class Graphic {
private:
	GLuint		m_tex_id;
	int*		m_tex_count;

	double		m_x;
	double		m_y;
	double		m_center_x;
	double		m_center_y;
	double		m_scale_x;
	double		m_scale_y;
	double		m_rotation;

	int		m_priority;
	bool		m_invisible;

	void		init(SDL_Surface* image);

protected:
	double		m_image_width;
	double		m_image_height;

	GLuint	get_texture_id() const;
	Graphic();
	Graphic(SDL_Surface* image);
	Graphic(const char* filename);
	Graphic(const Graphic& other);

	void transform_gl() const;

public:
	enum Priority {
		FOREGROUND = -256,
		OBSTACLE = 256,
		BACKGROUND = 512
	};
	enum HudPriority {
		TEXT = -256
	};

	virtual		~Graphic();
	virtual	Graphic* clone() const = 0;
	
	double		get_image_width() const;
	double		get_image_height() const;

	double		get_x() const;
	double		get_y() const;
	double		get_scale_x() const;
	double		get_scale_y() const;
	double		get_rotation() const;

	int		get_priority() const;
	void		set_priority(int priority);

	void		set_x(double x);
	void		set_y(double y);
	void		set_scale_x(double scale_x);
	void		set_scale_y(double scale_y);
	void		set_rotation(double rotation);

	double	get_center_x() const;
	double	get_center_y() const;

	void	set_center_x(double center_x);
	void	set_center_y(double center_y);

	bool		is_invisible() const;
	void		set_invisible(bool invisible);

	void		set_color_intensity(const Color& color); // Does not affect alpha
	virtual void	set_red_intensity(double r) = 0;
	virtual void	set_green_intensity(double g) = 0;
	virtual void	set_blue_intensity(double b) = 0;
	virtual void	set_alpha(double alpha) = 0;

	virtual Graphic*	get_graphic(const std::string&) { return NULL; }

	virtual void	draw(const GameWindow* window) const = 0;
};

#endif
