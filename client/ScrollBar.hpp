/*
 * client/ScrollBar.hpp
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

#ifndef LM_CLIENT_SCROLLBAR_HPP
#define LM_CLIENT_SCROLLBAR_HPP

#include "TableBackground.hpp"
#include "GameWindow.hpp"

class ScrollArea;
class ScrollBar {
public:
	enum ScrollWidget {
		NO_WIDGET = 0,
		TOP_BUTTON,
		TOP_TRACK,
		TRACKER,
		BOTTOM_TRACK,
		BOTTOM_BUTTON,

		BUTTONS,
		TRACK
	};
	enum {
		SCROLL_WIDTH = 15
	};

private:
	TableBackground m_bg;
	double		m_height;
	ScrollArea*	m_linked;
	GameWindow*	m_window;
	double		m_scrollbar_height;
	double		m_progress;
	ScrollWidget	m_pressed;
	double		m_grab_y;
	bool		m_updated;
	double		m_scroll_speed;
	double		m_track_speed;

	const static double DEFAULT_AUTOSCROLL;

public:
	ScrollBar(ScrollArea* area = NULL);

	void	set_x(double x);
	void	set_y(double y);
	void	set_height(double height);
	
	double	get_x() const;
	double	get_y() const;
	double	get_height() const;

	void	mouse_button_event(const SDL_MouseButtonEvent& event);
	void	mouse_motion_event(const SDL_MouseMotionEvent& event);

	// Scroll based on what's pressed down--use with mouse being held
	void	autoscroll(double scale);

	void	set_scroll_progress(double amount);
	void	scroll(double amount);
	double	get_scroll_progress() const;

	void	set_scrollbar_height(double height);
	void	set_section_color(ScrollWidget section, Color color);
	void	set_border_color(Color color);
	void	set_border_width(double width);

	void	set_scroll_speed(double speed);
	void	set_track_speed(double speed);

	double	get_scroll_speed() const;
	double	get_track_speed() const;

	void	register_window(GameWindow* window);
	void	unregister_window();

	void	relink(ScrollArea* linked);
	ScrollArea* getLinked();
};

#endif
