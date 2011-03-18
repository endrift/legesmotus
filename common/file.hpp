/*
 * common/file.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#ifndef LM_COMMON_FILE_HPP
#define LM_COMMON_FILE_HPP

#include "misc.hpp"
#include <iostream>

#ifdef __WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

namespace LM {
	void open_resource(std::ifstream* file, const char* filename, bool binary = false);
	const char* resource_dir();
	const char* user_dir();
	
	void read8(std::istream* f, uint8_t* v);
	void read8(std::istream* f, int8_t* v);
	void read16(std::istream* f, uint16_t* v);
	void read16(std::istream* f, int16_t* v);
	void read32(std::istream* f, uint32_t* v);
	void read32(std::istream* f, int32_t* v);
	void read32(std::istream* f, float* v);
	void expect(std::istream* f, const void* v, int n);
	
	void write8(std::ostream* f, uint8_t v);
	void write8(std::ostream* f, int8_t v);
	void write16(std::ostream* f, uint16_t v);
	void write16(std::ostream* f, int16_t v);
	void write16(std::ostream* f, int v);
	void write32(std::ostream* f, uint32_t v);
	void write32(std::ostream* f, int32_t v);
	void write32(std::ostream* f, float v);
	void write0(std::ostream* f, int n);
}

#endif
