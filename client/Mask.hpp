/*
 * client/Mask.hpp
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

#ifndef LM_CLIENT_MASK_HPP
#define LM_CLIENT_MASK_HPP

#include "Graphic.hpp"

class Mask : public Graphic {
private:
	Graphic* m_mask;
	Graphic* m_masked;
public:
	Mask(Graphic* mask, Graphic* masked);
	Mask(const Mask& other);
	virtual		~Mask();
	virtual Mask* clone() const;

	Graphic*	get_mask();
	Graphic*	get_masked();
	
	void		set_mask(Graphic* mask);
	void		set_masked(Graphic* masked);

	virtual void	set_alpha(double alpha) { (void)(alpha); }
	virtual void	set_red_intensity(double r) { (void)(r); }
	virtual void	set_green_intensity(double g) { (void)(g); }
	virtual void	set_blue_intensity(double b) { (void)(b); }

	virtual void	draw(const GameWindow* window) const;
};

#endif
