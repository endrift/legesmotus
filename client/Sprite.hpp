/*
 * client/Sprite.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_SPRITE_HPP
#define LM_CLIENT_SPRITE_HPP

#include "compat_gl.h"
#include "common/compat_sdl.h"

class GameWindow;
class Sprite {
private:
	SDL_Surface*	m_image;
	GLuint		m_tex_id;
	int		m_width;
	int		m_height;
	int		m_image_width;
	int		m_image_height;
	int*		m_tex_count;

	double		m_x;
	double		m_y;
	int		m_priority;

	double		m_scale_x;
	double		m_scale_y;
	double		m_rotation;

	double		m_center_x;
	double		m_center_y;

	double		m_alpha;

	void		init(SDL_Surface* image);

public:
	Sprite(SDL_Surface* image);
	Sprite(const char* filename);
	Sprite(const Sprite& other);
	~Sprite();

	GLuint	get_texture_id() const;
	int	get_width() const;
	int	get_height() const;
	
	int	get_image_width() const;
	int	get_image_height() const;

	double	get_x() const;
	double	get_y() const;
	int	get_priority() const;

	double	get_scale_x() const;
	double	get_scale_y() const;
	double	get_rotation() const;

	double	get_center_x() const;
	double	get_center_y() const;

	double	get_alpha() const;

	void	set_x(double x);
	void	set_y(double y);
	void	set_priority(int priority);

	void	set_scale_x(double scale_x);
	void	set_scale_y(double scale_y);
	void	set_rotation(double rotation);

	void	set_center_x(double center_x);
	void	set_center_y(double center_y);

	void	set_alpha(double alpha);

	void	draw(const GameWindow* window) const;
};

#endif
