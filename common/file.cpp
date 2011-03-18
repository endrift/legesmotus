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

#include <fstream>
#include <sstream>

using namespace LM;
using namespace std;

#ifdef __WIN32

#include <Shlobj.h>

// MinGW doesn't have this defined
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif

#else

#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

#endif

void LM::open_resource(ifstream* file, const char* filename, bool binary) {
	ASSERT(file != NULL);
	ios_base::openmode mode = ios_base::in;
	if (binary) {
		mode |= ios_base::binary;
	}

	stringstream s;

	s << user_dir();
	s << filename;
	file->open(s.str().c_str(), mode);

	if (file->fail()) {
		// Could not open the user resource
		file->close();
		s.str("");
		s << resource_dir();
		s << filename;
		file->open(s.str().c_str(), mode);
	}
}

void LM::open_for_writing(ofstream* file, const char* filename, bool binary) {
	ASSERT(file != NULL);
	ios_base::openmode mode = ios_base::out | ios_base::trunc;
	if (binary) {
		mode |= ios_base::binary;
	}

	stringstream s;

	s << user_dir();
	s << filename;
	file->open(s.str().c_str(), mode);
}

const char* LM::resource_dir() {
	static string dir("");

	if (!dir.empty()) {
		return dir.c_str();
	}

	const char* envdir = getenv("LM_DATA_DIR");
	if (envdir != NULL) {
		dir = envdir;
	} else {
		dir = LM_DATA_DIR;
	}
	dir.append(PATH_SEP);

	return dir.c_str();
}

const char* LM::user_dir() {
	static string dir("");

	if (!dir.empty()) {
		return dir.c_str();
	}

#ifdef __WIN32
	static TCHAR path[MAX_PATH];
	HRESULT err = SHGetFolderPath(0, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
	if (SUCCEEDED(err)) {
		dir = path;
		dir += "\\Leges Motus\\";
		if (!CreateDirectory(cfg.c_str(), NULL)) {
			if (GetLastError() != ERROR_ALREADY_EXISTS) {
				WARN("Couldn't create config directory");
			}
		}
	} else {
		WARN("Couldn't obtain application settings directory");
	}
#else
	if (struct passwd* pw = getpwuid(getuid())) {
		dir = pw->pw_dir;
		dir += "/.legesmotus/";
		int err = mkdir(dir.c_str(), 0775);
		if (err && errno != EEXIST) {
			WARN("Couldn't create config directory");
		}
	} else {
		WARN("Couldn't obtain application settings directory");
	}
#endif

	return dir.c_str();
}

void LM::read8(std::istream* f, uint8_t* v) {
	f->read((char*) v, 1);
}

void LM::read8(std::istream* f, int8_t* v) {
	read8(f, (uint8_t*) v);
}

void LM::read16(std::istream* f, uint16_t* v) {
	uint8_t* va = (uint8_t*) v;
	uint8_t in[2];
	f->read((char*) in, 2);
	va[0] = in[0];
	va[1] = in[1];
}

void LM::read16(std::istream* f, int16_t* v) {
	read16(f, (uint16_t*) v);
}

void LM::read32(std::istream* f, uint32_t* v) {
	uint8_t* va = (uint8_t* )v;
	uint8_t in[4];
	f->read((char*) in, 4);
	va[0] = in[0];
	va[1] = in[1];
	va[2] = in[2];
	va[3] = in[3];
}

void LM::read32(std::istream* f, int32_t* v) {
	read32(f, (uint32_t*) v);
}

void LM::read32(std::istream* f, float* v) {
	read32(f, (uint32_t*) v);
}

void LM::expect(std::istream* f, const void* v, int n) {
	char* va = (char*) v;
	char in;
	for (int i = 0; i < n; ++i) {
		f->read(&in, 1);
		if (in != va[i]) {
			throw Exception("Expect failed");
		}
	}
}

void LM::write8(std::ostream* f, uint8_t v) {
	f->write((const char*) &v, 1);
}

void LM::write16(std::ostream* f, uint16_t v) {
	uint8_t* va = (uint8_t*) &v;
	uint8_t in[2];
	in[0] = va[0];
	in[1] = va[1];
	f->write((const char*) in, 2);
}

void LM::write16(std::ostream* f, int16_t v) {
	write16(f, (uint16_t) v);
}

void LM::write16(std::ostream* f, int v) {
	write16(f, (uint16_t) v);
}

void LM::write32(std::ostream* f, uint32_t v) {
	uint8_t* va = (uint8_t*) &v;
	uint8_t in[2];
	in[0] = va[0];
	in[1] = va[1];
	in[2] = va[2];
	in[3] = va[3];
	f->write((const char*) in, 4);
}

void LM::write32(std::ostream* f, int32_t v) {
	write32(f, (uint32_t) v);
}

void LM::write32(std::ostream* f, float v) {
	write32(f, *(uint32_t *) &v);
}

void LM::write0(std::ostream* f, int n) {
	static const char z[16] = {};
	for (int i = 0; i < n; i += sizeof(z)) {
		f->write(z, min<int>(n - i, sizeof(z)));
	}
}
