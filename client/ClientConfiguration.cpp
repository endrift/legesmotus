/*
 * client/ClientConfiguration.cpp
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

#include "ClientConfiguration.hpp"
#include "common/ConfigManager.hpp"
#include <iostream>

using namespace LM;
using namespace std;

ClientConfiguration::ClientConfiguration() {
	m_string_defaults["name"] = "Unnamed";
	m_int_defaults["multisample"] = 0;
	m_int_defaults["screen_width"] = 800;
	m_int_defaults["screen_height"] = 600;
	m_bool_defaults["fullscreen"] = false;
	m_bool_defaults["vsync"] = true;
	m_bool_defaults["text_shadow"] = false;
	m_bool_defaults["text_background"] = true;
	m_bool_defaults["text_sliding"] = true;
	m_bool_defaults["sound"] = true;
	reload_config();
}

ClientConfiguration::~ClientConfiguration() {
	
}

void ClientConfiguration::reload_config() {
	m_config_manager.load_system_config();
	m_config_manager.load_personal_config();
	
	map<string, string>::iterator sit;
	for ( sit=m_string_defaults.begin() ; sit != m_string_defaults.end(); sit++ ) {
		string itemname = (*sit).first;
		string itemdefault = (*sit).second;
		if (m_config_manager.has(itemname.c_str())) {
			m_string_vals[itemname] = m_config_manager.get<string>(itemname.c_str());
		} else {
			m_string_vals[itemname] = itemdefault;
			m_config_manager.set(itemname.c_str(), itemdefault);
		}
	}
	
	map<string, int>::iterator iit;
	for ( iit=m_int_defaults.begin() ; iit != m_int_defaults.end(); iit++ ) {
		string itemname = (*iit).first;
		int itemdefault = (*iit).second;
		if (m_config_manager.has(itemname.c_str())) {
			m_int_vals[itemname] = m_config_manager.get<int>(itemname.c_str());
		} else {
			m_int_vals[itemname] = itemdefault;
			m_config_manager.set(itemname.c_str(), itemdefault);
		}
	}
	
	map<string, bool>::iterator bit;
	for ( bit=m_bool_defaults.begin() ; bit != m_bool_defaults.end(); bit++ ) {
		string itemname = (*bit).first;
		bool itemdefault = (*bit).second;
		if (m_config_manager.has(itemname.c_str())) {
			m_bool_vals[itemname] = m_config_manager.get<bool>(itemname.c_str());
		} else {
			m_bool_vals[itemname] = itemdefault;
			m_config_manager.set(itemname.c_str(), itemdefault);
		}
	}
	
	m_config_manager.save_personal_config();
}

void ClientConfiguration::save_config() {
	m_config_manager.save_personal_config();
}

void ClientConfiguration::reset_to_defaults() {
	map<string, string>::iterator sit;
	for ( sit=m_string_defaults.begin() ; sit != m_string_defaults.end(); sit++ ) {
		string itemname = (*sit).first;
		string itemdefault = (*sit).second;
		m_string_vals[itemname] = itemdefault;
		m_config_manager.set(itemname.c_str(), itemdefault);
	}
	
	map<string, int>::iterator iit;
	for ( iit=m_int_defaults.begin() ; iit != m_int_defaults.end(); iit++ ) {
		string itemname = (*iit).first;
		int itemdefault = (*iit).second;
		m_int_vals[itemname] = itemdefault;
		m_config_manager.set(itemname.c_str(), itemdefault);
	}
	
	map<string, bool>::iterator bit;
	for ( bit=m_bool_defaults.begin() ; bit != m_bool_defaults.end(); bit++ ) {
		string itemname = (*bit).first;
		bool itemdefault = (*bit).second;
		m_bool_vals[itemname] = itemdefault;
		m_config_manager.set(itemname.c_str(), itemdefault);
	}
	
	save_config();
}

string ClientConfiguration::get_string_value(string name) {
	if (m_string_vals.find(name) != m_string_vals.end()) {
		return m_string_vals[name];
	} else {
		return "";
	}
}

int ClientConfiguration::get_int_value(string name) {
	if (m_int_vals.find(name) != m_int_vals.end()) {
		return m_int_vals[name];
	} else {
		return -1;
	}
}

bool ClientConfiguration::get_bool_value(string name) {
	if (m_bool_vals.find(name) != m_bool_vals.end()) {
		return m_bool_vals[name];
	} else {
		return false;
	}
}

void ClientConfiguration::set_string_value(string name, string value) {
	if (m_string_defaults.find(name) != m_string_defaults.end()) {
		m_string_vals[name] = value;
		m_config_manager.set(name.c_str(), value);
		save_config();
	}
}

void ClientConfiguration::set_int_value(string name, int value) {
	if (m_int_defaults.find(name) != m_int_defaults.end()) {
		m_int_vals[name] = value;
		m_config_manager.set(name.c_str(), value);
		save_config();
	}
}

void ClientConfiguration::set_bool_value(string name, bool value) {
	if (m_bool_defaults.find(name) != m_bool_defaults.end()) {
		m_bool_vals[name] = value;
		m_config_manager.set(name.c_str(), value);
		save_config();
	}
}
