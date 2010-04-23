/*
 * common/WeaponFile.hpp
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

#ifndef LM_COMMON_WEAPONFILE_HPP
#define LM_COMMON_WEAPONFILE_HPP

#include <string>
#include <list>
#include <iosfwd>
#include "common/WeaponReader.hpp"

namespace LM {
	class WeaponFile {
	private:
		std::string		m_name;
		std::list<WeaponReader>	m_weapons;

	public:
		bool	is_empty() const { return m_weapons.empty(); }
		bool	is_filled() const { return !m_weapons.empty(); }

		// Read and parse the given input stream and load into the current weapon set
		bool	load(const char* name, std::istream& in);

		// load_file will preserve the current weapon set if it can't open the new set
		// (and return false to indicate error)
		bool	load_file(const char* name, const char* path);

		// Remove all weapons from the set:
		void	clear();

		const std::list<WeaponReader>	get_weapons() const { return m_weapons; }
	};
}

#endif
