/*
 * client/Radar.hpp
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

#ifndef LM_CLIENT_RADAR_HPP
#define LM_CLIENT_RADAR_HPP

#include "Mask.hpp"
#include "Sprite.hpp"
#include "GraphicGroup.hpp"
#include "common/PathManager.hpp"
#include "common/GameParameters.hpp"
#include "TransitionManager.hpp"
#include <stdint.h>
#include <map>

namespace LM {
	class Radar {
	private:
		PathManager&	m_path_manager;
		Mask*		m_minimask;
		GraphicGroup*	m_minigroup;
		GraphicGroup*	m_whole;
		Sprite*		m_master_blip_red;
		Sprite*		m_master_blip_blue;
		double		m_scale;
		TransitionManager m_transman;
		RadarMode	m_mode;

	public:
		explicit Radar(PathManager& path_manager, double scale, RadarMode mode);
		~Radar();
	
		void	set_x(double x);
		void	set_y(double y);
		void	set_invisible(bool enable);
		void	set_scale(double scale);
		void	set_mode(RadarMode mode);

		RadarMode get_mode() const;
	
		// Blips cannot be accessed directly
		void	add_blip(uint32_t id, char team, double x, double y);
		void	move_blip(uint32_t id, double x, double y);
		void	set_blip_invisible(uint32_t id, bool invisible);
		void	set_blip_alpha(uint32_t id, double alpha);
		void	activate_blip(uint32_t id, uint64_t current, uint64_t duration);
		void	remove_blip(uint32_t id);
	
		void	recenter(double x, double y);
		void	update(uint64_t tick);

		void	register_with_window(GameWindow* window);
		void	unregister_with_window(GameWindow* window);
	};
}

#endif
