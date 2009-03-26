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

class GameWindow;
class Graphic {
private:
	SDL_Surface*	m_image;
	GLuint		m_tex_id;
	int*		m_tex_count;
	int		m_image_width;
	int		m_image_height;

	double		m_x;
	double		m_y;
	double		m_scale_x;
	double		m_scale_y;
	double		m_rotation;

	int		m_priority;

	void		init(SDL_Surface* image);

protected:
	GLuint	get_texture_id() const;
	Graphic();
	Graphic(SDL_Surface* image);
	Graphic(const char* filename);
	Graphic(const Graphic& other);

	void transform_gl() const;

public:
	virtual		~Graphic();
	
	int		get_image_width() const;
	int		get_image_height() const;

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


	virtual void	draw(const GameWindow* window) const = 0;
};

#endif
