/*
 * client/ScrollArea.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

namespace LM {
	class ScrollBar;
	class ScrollArea : public Graphic {
	private:
		GraphicGroup	m_group;
		double		m_horiz_progress;
		double		m_vert_progress;
		ScrollBar*	m_horiz_linked;
		ScrollBar*	m_vert_linked;
		double		m_width;
		double		m_height;
		double		m_content_height;
		double		m_content_width;
		bool		m_updated;
	
	public:
		ScrollArea(double width, double height, double content_width, double content_height, ScrollBar* hbar = NULL, ScrollBar* vbar = NULL);
		ScrollArea(const ScrollArea& other);
		virtual ScrollArea* clone() const;

		virtual void	set_width(double width);
		virtual void	set_height(double height);
		void	set_content_height(double height);
		void	set_content_width(double height);

		double	get_width() const;
		double	get_height() const;
		double	get_content_height() const;
		double	get_content_width() const;

		// TODO: Inner class to reduce code reuse?
		void	set_horiz_scroll_progress(double amount);
		void	horiz_scroll(double amount);

		void	set_horiz_scroll_progress_pixels(double pixels);
		void	horiz_scroll_pixels(double pixels);

		double	get_horiz_scroll_progress() const;
		double	get_horiz_scroll_progress_pixels() const;

		void	set_vert_scroll_progress(double amount);
		void	vert_scroll(double amount);

		void	set_vert_scroll_progress_pixels(double pixels);
		void	vert_scroll_pixels(double pixels);

		double	get_vert_scroll_progress() const;
		double	get_vert_scroll_progress_pixels() const;

		GraphicGroup*	get_group();

		void	horiz_relink(ScrollBar* linked);
		ScrollBar* get_horiz_linked();

		void	vert_relink(ScrollBar* linked);
		ScrollBar* get_vert_linked();
	
		virtual void draw(const GameWindow* window) const;
	};
}

#endif
