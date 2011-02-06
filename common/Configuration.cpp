/*
 * common/Configuration.cpp
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

#include "Configuration.hpp"
#include "misc.hpp"

using namespace LM;
using namespace std;

#ifndef __WIN32

#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

#endif

const string& Configuration::local_dir() {
	static string cfg("");

	if (cfg.empty()) {
#ifdef __WIN32
		static TCHAR path[MAX_PATH];
		HRESULT err = SHGetKnownFolderPath(0, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
		if (SUCCEEDED) {
			cfg = path;
			cfg += "\\Leges Motus\\";
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
			cfg = pw->pw_dir;
			cfg += "/.legesmotus/";
			int err = mkdir(cfg.c_str(), 0775);
			if (err && err != EEXIST) {
				WARN("Couldn't create config directory");
			}
		} else {
			WARN("Couldn't obtain application settings directory");
		}
#endif
	}

	return cfg;
}

const string& Configuration::global_dir() {
	static string cfg("");

	if (cfg.empty()) {
#ifdef __WIN32
		cfg = ".\\";
#else
		// TODO make this a build flag
		cfg = LM_CONFIG_DIR "/";
#endif
	}

	return cfg;
}

bool Configuration::key_exists(CSimpleIniW* dict, const wchar_t* section, const wchar_t* key) {
	if (dict == NULL) {
		return false;
	}

	const CSimpleIniW::TKeyVal* keys = dict->GetSection(section);

	return keys != NULL && (keys->find(key) != keys->end());
}

Configuration::Configuration() {
	m_local = new CSimpleIniW(true);
	m_global = NULL;
}

Configuration::Configuration(const std::string& filename) {
	m_local = new CSimpleIniW(true);
	m_global = new CSimpleIniW(true);

	// TODO error checking
	m_local->LoadFile((local_dir() + filename).c_str());
	m_global->LoadFile((global_dir() + filename).c_str());
}

Configuration::~Configuration() {
	delete m_local;
	delete m_global;
}

bool Configuration::local_key_exists(const wchar_t* section, const wchar_t* key) const {
	return key_exists(m_local, section, key);
}

bool Configuration::global_key_exists(const wchar_t* section, const wchar_t* key) const {
	return key_exists(m_global, section, key);
}

bool Configuration::key_exists(const wchar_t* section, const wchar_t* key) const {
	return global_key_exists(section, key) || local_key_exists(section, key);
}

const wchar_t* Configuration::get_string(const wchar_t* section, const wchar_t* key, const wchar_t* dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetValue(section, key, dflt);
	}
	return m_global->GetValue(section, key, dflt);
}

int Configuration::get_int(const wchar_t* section, const wchar_t* key, int dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetLongValue(section, key, dflt);
	}
	return m_global->GetLongValue(section, key, dflt);
}

bool Configuration::get_bool(const wchar_t* section, const wchar_t* key, bool dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetBoolValue(section, key, dflt);
	}
	return m_global->GetBoolValue(section, key, dflt);
}

float Configuration::get_float(const wchar_t* section, const wchar_t* key, float dflt) const {
	const wchar_t* val = m_local->GetValue(section, key);

	if (val == NULL) {
		val = m_global->GetValue(section, key);
	}
	if (val == NULL) {
		return dflt;
	}

	return wtof(val);
}

