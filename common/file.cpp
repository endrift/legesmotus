/*
 * common/file.cpp
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

#include "file.hpp"

using namespace LM;
using namespace std;

void read8(std::istream* f, uint8_t* v) {
	f->read((char*) v, 1);
}

void read8(std::istream* f, int8_t* v) {
	read8(f, (uint8_t*) v);
}

void read16(std::istream* f, uint16_t* v) {
	uint8_t* va = (uint8_t*) v;
	uint8_t in[2];
	f->read((char*) in, 2);
	va[0] = in[0];
	va[1] = in[1];
}

void read16(std::istream* f, int16_t* v) {
	read16(f, (uint16_t*) v);
}

void read32(std::istream* f, uint32_t* v) {
	uint8_t* va = (uint8_t* )v;
	uint8_t in[4];
	f->read((char*) in, 4);
	va[0] = in[0];
	va[1] = in[1];
	va[2] = in[2];
	va[3] = in[3];
}

void read32(std::istream* f, int32_t* v) {
	read32(f, (uint32_t*) v);
}

void read32(std::istream* f, float* v) {
	read32(f, (uint32_t*) v);
}

void expect(std::istream* f, const void* v, int n) {
	char* va = (char*) v;
	char in;
	for (int i = 0; i < n; ++i) {
		f->read(&in, 1);
		if (in != va[i]) {
			throw Exception("Expect failed");
		}
	}
}

void write8(std::ostream* f, uint8_t v) {
	f->write((const char*) &v, 1);
}

void write16(std::ostream* f, uint16_t v) {
	uint8_t* va = (uint8_t*) &v;
	uint8_t in[2];
	in[0] = va[0];
	in[1] = va[1];
	f->write((const char*) in, 2);
}

void write16(std::ostream* f, int16_t v) {
	write16(f, (uint16_t) v);
}

void write16(std::ostream* f, int v) {
	write16(f, (uint16_t) v);
}

void write32(std::ostream* f, uint32_t v) {
	uint8_t* va = (uint8_t*) &v;
	uint8_t in[2];
	in[0] = va[0];
	in[1] = va[1];
	in[2] = va[2];
	in[3] = va[3];
	f->write((const char*) in, 4);
}

void write32(std::ostream* f, int32_t v) {
	write32(f, (uint32_t) v);
}

void write32(std::ostream* f, float v) {
	write32(f, *(uint32_t *) &v);
}

void write0(std::ostream* f, int n) {
	static const char z[16] = {};
	for (int i = 0; i < n; i += sizeof(z)) {
		f->write(z, min<int>(n - i, sizeof(z)));
	}
}
