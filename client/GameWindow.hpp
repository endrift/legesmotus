/*
 * GameWindow.hpp
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
 
#ifndef LM_CLIENT_GAMEWINDOW_HPP
#define LM_CLIENT_GAMEWINDOW_HPP

#include "SDL.h"
#include "GraphicGroup.hpp"

class GameWindow {
private:
	static GameWindow*	m_instance;
	SDL_Surface*		m_context;
	int			m_width;
	int			m_height;
	int			m_depth;
	bool			m_fullscreen;
	
	std::list<Graphic*>		m_graphics;
	std::list<Graphic*>		m_hud_graphics;

	static SDL_Surface*	m_icon;

	double			m_offset_x;
	double			m_offset_y;

	GameWindow(int width, int height, int depth, bool fullscreen);
	~GameWindow();
public:

	// Initialize the video subsystem, if it hasn't been already
	// This will return true iff the subsystem is initialized
	// NB: It will return true if the system was already initialized
	static bool		init_video();
	static void		deinit_video();
	// This must be called before an instance is obtained
	static void		set_icon(SDL_Surface* icon);

	// Will get the current GameWindow, adjusted to these dimensions, if one exists
	// If it doesn't exist, it will construct it
	// Depth cannot be changed while running
	static GameWindow*	get_instance(int width, int height, int depth, bool fullscreen);
	// Get the current instance, or NULL if there is no such instance
	static GameWindow*	get_instance();
	static GameWindow*	get_optimal_instance();
	static void		destroy_instance();

	// Pass NULL, NULL to get depth and number of modes, or pointers for all to get all the modes
	// If num_modes is less than the actual number, only some are added, with no particular distinction
	// These are not necessarily sorted at all
	// NB: Video must be initialized before this function is called. Most functions will call it themselves first.
	static void		supported_resolutions(int* widths, int* heights, int* depth, size_t* num_modes);

	int			get_width() const;
	int			get_height() const;
	int			get_depth() const;
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

