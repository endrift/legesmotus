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

#include <cstring>

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

class ConfigIterator : public ConstIterator<pair<const char*, const char*> >::OpaqueIterator {
private:
	const CSimpleIniA::TKeyVal* m_local;
	const CSimpleIniA::TKeyVal* m_global;

	CSimpleIniA::TKeyVal::const_iterator m_liter;
	CSimpleIniA::TKeyVal::const_iterator m_giter;

public:
	ConfigIterator(const CSimpleIniA::TKeyVal* local, const CSimpleIniA::TKeyVal* global);

	virtual bool has_more() const;
	virtual pair<const char*, const char*> next();
	virtual ConfigIterator* clone();
};

ConfigIterator::ConfigIterator(const CSimpleIniA::TKeyVal* local, const CSimpleIniA::TKeyVal* global) {
	m_local = local;
	m_global = global;

	if (m_local != NULL) {
		m_liter = local->begin();
	}
	if (m_global != NULL) {
		m_giter = global->begin();
	}
}

bool ConfigIterator::has_more() const {
	return (m_local != NULL && m_liter != m_local->end()) || (m_global != NULL && m_giter != m_global->end());
}

pair<const char*, const char*> ConfigIterator::next() {
	pair<const char*, const char*> lnext(NULL, NULL);
	pair<const char*, const char*> gnext(NULL, NULL);
	pair<const char*, const char*> next(NULL, NULL);

	if (m_local != NULL && m_liter != m_local->end()) {
		lnext.first = m_liter->first.pItem;
		lnext.second = m_liter->second;
	}

	if (m_global != NULL && m_giter != m_global->end()) {
		gnext.first = m_giter->first.pItem;
		gnext.second = m_giter->second;
	}

	// Continue in lockstep
	// We can do this because they're both sorted on the key
	if (lnext.first == NULL) {
		if (gnext.first != NULL) {
			next = gnext;
			++m_giter;
		}
		// Otherwise they're both null, so fall through
	} else if (gnext.first == NULL) {
		next = lnext;
		++m_liter;
	} else {
		// Neither are null, so let's see which one we should return
		int cmp = strcmp(lnext.first, gnext.first);
		if (cmp < 0) {
			next = lnext;
			++m_liter;
		} else if (cmp > 0) {
			next = gnext;
			++m_giter;
		} else {
			// Local overrides global! Step both but return local
			next = lnext;
			++m_liter;
			++m_giter;
		}
	}

	return next;
}

ConfigIterator* ConfigIterator::clone() {
	return new ConfigIterator(*this);
}

const string& Configuration::local_dir() {
	static string cfg("");

	if (cfg.empty()) {
#ifdef __WIN32
		static TCHAR path[MAX_PATH];
		HRESULT err = SHGetFolderPath(0, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
		if (SUCCEEDED(err)) {
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
			if (err && errno != EEXIST) {
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
		cfg = LM_CONFIG_DIR "/";
#endif
	}

	return cfg;
}

bool Configuration::key_exists(CSimpleIniA* dict, const char* section, const char* key) {
	if (dict == NULL) {
		return false;
	}

	const CSimpleIniA::TKeyVal* keys = dict->GetSection(section);

	return keys != NULL && (keys->find(key) != keys->end());
}

Configuration::Configuration() {
	m_local = new CSimpleIniA(true);
	m_global = NULL;
}

Configuration::Configuration(const std::string& filename) {
	m_local = new CSimpleIniA(true);
	m_global = new CSimpleIniA(true);

	// TODO error checking
	m_local->LoadFile((local_dir() + filename).c_str());
	m_global->LoadFile((global_dir() + filename).c_str());
}

Configuration::~Configuration() {
	delete m_local;
	delete m_global;
}

bool Configuration::local_key_exists(const char* section, const char* key) const {
	return key_exists(m_local, section, key);
}

bool Configuration::global_key_exists(const char* section, const char* key) const {
	return key_exists(m_global, section, key);
}

bool Configuration::key_exists(const char* section, const char* key) const {
	return global_key_exists(section, key) || local_key_exists(section, key);
}

ConstIterator<pair<const char*, const char*> > Configuration::get_section(const char* section) const {
	return ConstIterator<pair<const char*, const char*> >(new ConfigIterator(m_local->GetSection(section), m_global->GetSection(section)));
}

const char* Configuration::get_string(const char* section, const char* key, const char* dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetValue(section, key, dflt);
	}
	return m_global->GetValue(section, key, dflt);
}

int Configuration::get_int(const char* section, const char* key, int dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetLongValue(section, key, dflt);
	}
	return m_global->GetLongValue(section, key, dflt);
}

bool Configuration::get_bool(const char* section, const char* key, bool dflt) const {
	if (local_key_exists(section, key)) {
		return m_local->GetBoolValue(section, key, dflt);
	}
	return m_global->GetBoolValue(section, key, dflt);
}

float Configuration::get_float(const char* section, const char* key, float dflt) const {
	const char* val = m_local->GetValue(section, key);

	if (val == NULL) {
		val = m_global->GetValue(section, key);
	}
	if (val == NULL) {
		return dflt;
	}

	return atof(val);
}

