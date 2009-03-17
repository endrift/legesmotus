/*
 * GameWindow.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMEWINDOW_HPP
#define LM_CLIENT_GAMEWINDOW_HPP

#include "common/compat_sdl.h"
#include "Sprite.hpp"
#include <list>

class GameWindow {
private:
	static GameWindow*	m_instance;
	SDL_Surface*		m_context;
	int			m_width;
	int			m_height;
	int			m_depth;
	bool			m_fullscreen;
	std::list<Sprite*>	m_sprites;

	GameWindow(int width, int height, int depth, bool fullscreen);
	~GameWindow();
public:

	// Will get the current GameWindow, adjusted to these dimensions, if one exists
	// If it doesn't exist, it will construct it
	// Depth cannot be changed while running
	static GameWindow*	get_instance(int width, int height, int depth, bool fullscreen);
	// Get the current instance, or NULL if there is no such instance
	static GameWindow*	get_instance();
	static void		destroy_instance();

	void			set_dimensions(int width, int height);
	void			set_fullscreen(bool fullscreen);

	int			get_width() const;
	int			get_height() const;
	bool			is_fullscreen() const;

	void			register_sprite(Sprite* sprite);

	void			redraw() const;
};

#endif

