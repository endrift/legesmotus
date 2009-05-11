/*
 * client/Graphic.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHIC_HPP
#define LM_CLIENT_GRAPHIC_HPP

#include "compat_gl.h"
#include "common/compat_sdl.h"
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
