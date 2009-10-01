/*
 * client/GraphicsCache.cpp
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

#include "GraphicsCache.hpp"
#include "Graphic.hpp"

using namespace LM;
using namespace std;

Graphic&	GraphicsCache::get_graphic(const char* path) {
	Graphic*&	graphic = m_cached_graphics[path];
	if (!graphic) {
		graphic = new Sprite(path);
	}
	return *graphic;
}

void	GraphicsCache::clear() {
	for (map<string, Graphic*>::iterator it(m_cached_graphics.begin()); it != m_cached_graphics.end(); ++it) {
		delete it->second;
	}
	m_cached_graphics.clear();
}

