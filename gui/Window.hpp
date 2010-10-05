/*
 * gui/Window.hpp
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

#ifndef LM_GUI_WINDOW_HPP
#define LM_GUI_WINDOW_HPP

#include "common/Exception.hpp"

namespace LM {
	class Image;
	class DrawContext;
	class Widget;

	class Window {
	public:
		enum Flags {
			FLAG_FULLSCREEN = 1,
			FLAG_VSYNC = 2
		};

	private:
		static Window*	m_instance;
		int	m_width;
		int	m_height;
		int	m_depth;
		bool	m_fullscreen;

	protected:
		static void set_instance(Window* instance);

		Window(int width, int height, int depth, int flags);
		virtual ~Window();

		void set_width(int width);
		void set_height(int height);
		void set_depth(int depth);

		virtual bool	rebuild(int width, int height, int depth, int flags) = 0;

		virtual void	set_icon_internal(Image* icon) = 0;

	public:
		static Window*	get_instance();
		static void		destroy_instance();

		static void		set_icon(Image* icon);

		int		get_width() const;
		int		get_height() const;
		int		get_depth() const;
		bool	is_fullscreen() const;

		bool	resize(int width, int height, int depth, int flags);

		virtual DrawContext*	get_context() = 0;

		void set_root_widget(Widget* root);
		Widget* get_root_widget();

		virtual void redraw();
	};

	class VmodeNotSupportedException : public Exception {
	private:
		int	m_width;
		int	m_height;
		int	m_depth;
		bool	m_fullscreen;

	public:
		VmodeNotSupportedException(const std::string& message, int width, int height, int depth, bool fullscreen);

		int	get_width() const throw() { return m_width; }
		int	get_height() const throw() { return m_height; }
		int	get_depth() const throw() { return m_depth; }
		bool	get_fullscreen() const throw() { return m_fullscreen; }
	};
}

#endif
