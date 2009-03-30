/*
 * GameWindow.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_CLIENT_GAMEWINDOW_HPP
#define LM_CLIENT_GAMEWINDOW_HPP

#include "common/compat_sdl.h"
#include "GraphicGroup.hpp"

class GameWindow {
private:
	static GameWindow*	m_instance;
	SDL_Surface*		m_context;
	int			m_width;
	int			m_height;
	int			m_depth;
	bool			m_fullscreen;
	
	GraphicGroup		m_graphics;
	GraphicGroup		m_hud_graphics;

	int			m_offset_x;
	int			m_offset_y;

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

	int			get_width() const;
	int			get_height() const;
	bool			is_fullscreen() const;

	double			get_offset_x() const;
	double			get_offset_y() const;

	void			set_dimensions(int width, int height);
	void			set_fullscreen(bool fullscreen);

	void			set_offset_x(double offset);
	void			set_offset_y(double offset);

	void			register_graphic(Graphic* graphic);
	void			unregister_graphic(Graphic* graphic);
	
	void			register_hud_graphic(Graphic* graphic);
	void			unregister_hud_graphic(Graphic* graphic);

	void			redraw() const;
};

#endif

