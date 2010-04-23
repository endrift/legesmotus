/*
 * client/Transition.hpp
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

#ifndef LM_CLIENT_TRANSITION_HPP
#define LM_CLIENT_TRANSITION_HPP

#include "Curve.hpp"
#include <stdint.h>

// TODO template?
namespace LM {
	class Graphic;
	class Transition {
	public:
		typedef void (Graphic::*Property)(double param);
		class Listener {
		public:
			virtual ~Listener() {}
			virtual void finished(Transition* which) = 0;
		};
	private:
		Graphic*	m_transed;
		Property	m_prop;
		Curve*		m_curve;
		uint64_t	m_start;
		uint64_t	m_duration;
		bool		m_curve_owned;
		bool		m_graphic_owned;
		Listener*	m_listener;
	public:
		Transition(Graphic* transitioned, Property property, Curve* curve = 0, uint64_t start = 0, uint64_t duration = 0);
		~Transition();
	
		void	set_start(uint64_t start);
		void	set_duration(uint64_t duration);
		void	set_curve(Curve* curve);
		void	set_curve_ownership(bool owned);
		void	set_graphic_ownership(bool owned);
	
		uint64_t get_start() const;
		uint64_t get_duration() const;
		Curve*	get_curve();
		Graphic* get_graphic();
		bool	get_curve_ownership() const;
		bool	get_graphic_ownership() const;

		void	set_listener(Listener* listener);
	
		// Returns true if current >= end
		bool update(uint64_t current);
		void change_curve(uint64_t current, Curve* curve, uint64_t duration = 0);
	};
}

#endif
