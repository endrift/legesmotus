/*
 * client/ScrollArea.hpp
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

#ifndef LM_CLIENT_SCROLLAREA_HPP
#define LM_CLIENT_SCROLLAREA_HPP

#include "Mask.hpp"
#include "GraphicGroup.hpp"

class ScrollBar;
class ScrollArea : public Graphic {
private:
	GraphicGroup*	m_group;
	double		m_progress;
	ScrollBar*	m_linked;
	double		m_width;
	double		m_height;
	double		m_content_height;
	bool		m_updated;

public:
	ScrollArea(double width, double height, double content_height, ScrollBar* bar = NULL);
	ScrollArea(const ScrollArea& other);
	virtual ~ScrollArea();
	virtual ScrollArea* clone() const;

	void	set_width(double width);
	void	set_height(double height);
	void	set_content_height(double height);

	double	get_width() const;
	double	get_height() const;
	double	get_content_height() const;

	void	set_scroll_progress(double amount);
	void	scroll(double amount);

	void	set_scroll_progress_pixels(double pixels);
	void	scroll_pixels(double pixels);

	GraphicGroup*	get_group();

	void	relink(ScrollBar* linked);
	ScrollBar* getLinked();

	virtual void	set_alpha(double alpha) { (void)(alpha); }
	virtual void	set_red_intensity(double r) { (void)(r); }
	virtual void	set_green_intensity(double g) { (void)(g); }
	virtual void	set_blue_intensity(double b) { (void)(b); }

	virtual void draw(const GameWindow* window) const;
};

#endif
