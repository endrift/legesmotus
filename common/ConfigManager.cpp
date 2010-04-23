/*
 * common/ConfigManager.cpp
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

#include "ConfigManager.hpp"
#include "StringTokenizer.hpp"
#include "misc.hpp"
#include <istream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits>
#ifdef __WIN32
// Necessary to get SHGFP_TYPE_CURRENT
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif
#include <Windows.h>
#include <shlobj.h>
#endif

using namespace LM;
using namespace std;

void	ConfigManager::write_option(ostream& out, const ConfigManager::map_type::value_type& option) {
	out << option.first << ' ' << option.second << '\n'; // TODO: format more nicely
}

bool	ConfigManager::load(const char* filename) {
	ifstream	in(filename);
	return in && load(in);
}

bool	ConfigManager::load(std::istream& in) {
	string			line;
	while (getline(in, line)) {
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}

		StringTokenizer	tokenize(line, " \t", true, 2);
		string		name;
		tokenize >> name;

		if (name.empty()) {
			continue;
		}

		tokenize >> m_options[name];
	}
	return true;
}

bool	ConfigManager::save(const char* filename) const {
	ofstream	out(filename);
	return out && save(out);
}

bool	ConfigManager::save(std::ostream& out) const {
	map_type::const_iterator it(m_options.begin());
	while (it != m_options.end()) {
		write_option(out, *it++);
	}
	return true;
}

bool	ConfigManager::load_system_config() {
	if (const char* path = system_config_path()) {
		return load(path);
	}
	return false;
}

bool	ConfigManager::load_personal_config() {
	if (const char* path = personal_config_path()) {
		return load(path);
	}
	return false;
}

bool	ConfigManager::save_personal_config() const {
	if (const char* path = personal_config_path()) {
		return save(path);
	}
	return false;
}


const string*	ConfigManager::lookup(const char* option_name) const {
	map_type::const_iterator	it(m_options.find(option_name));
	return it != m_options.end() ? &it->second : NULL;
}

const string&	ConfigManager::operator[](const char* option_name) const {
	if (const string* option_value = lookup(option_name)) {
		return *option_value;
	} else {
		return make_empty<string>();
	}
}


template<> string	ConfigManager::get(const char* option_name) const {
	const string* raw_value = lookup(option_name);
	return raw_value ? *raw_value : string();
}

template<> const char*	ConfigManager::get(const char* option_name) const {
	const string* raw_value = lookup(option_name);
	return raw_value ? raw_value->c_str() : "";
}


template<> bool ConfigManager::get(const char* option_name) const {
	const string* raw_value = lookup(option_name);

	return raw_value &&
		(atoi(raw_value->c_str()) > 0 ||
		 strcasecmp(raw_value->c_str(), "yes") == 0 ||
		 strcasecmp(raw_value->c_str(), "true") == 0 ||
		 strcasecmp(raw_value->c_str(), "on") == 0);
}

template<> void ConfigManager::set(const char* option_name, const bool& option_value) {
	m_options[option_name] = option_value ? "yes" : "no";
}

template<> uint64_t	ConfigManager::get(const char* option_name) const {
	uint64_t	value = uint64_t();
	const string*	raw_value = lookup(option_name);
	if (raw_value && strcasecmp(raw_value->c_str(), "forever") == 0) {
		value = numeric_limits<uint64_t>::max();
	} else if (raw_value) {
		istringstream(*raw_value) >> value;
	}
	return value;
}

template<> void	ConfigManager::set(const char* option_name, const uint64_t& option_value) {
	if (option_value == numeric_limits<uint64_t>::max()) {
		m_options[option_name] = "forever";
	} else {
		ostringstream	out;
		out << option_value;
		m_options[option_name] = out.str();
	}
}


template<> void ConfigManager::set(const char* option_name, const char* const& option_value) {
	m_options[option_name] = option_value;
}

const char*	ConfigManager::personal_config_path() {
	static string	path;
	if (path.empty()) {
#ifdef __WIN32
		TCHAR	appdata_path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata_path))) {
			path = appdata_path;
			path += "\\legesmotusrc.txt";
		} else {
			return NULL;
		}
#else
		if (const char* home_dir = getenv("HOME")) {
			path = home_dir;
			path += "/.legesmotusrc";
		} else {
			return NULL;
		}
#endif
	}
	return path.c_str();
}

const char*	ConfigManager::system_config_path() { // TODO: use PREFIX/etc/legesmotusrc on UNIX
#ifdef __WIN32
	return "legesmotusrc.txt";
#else
	return "/etc/legesmotusrc";
#endif
}

