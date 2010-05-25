/*
 * common/ConfigManager.hpp
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

#ifndef LM_COMMON_CONFIGMANAGER_HPP
#define LM_COMMON_CONFIGMANAGER_HPP

#include <map>
#include <string>
#include <sstream>
#include <iosfwd>
#include <stdint.h>

/*
 * Example of use:
 * 
 * The file ~/.legesmotusrc contains:
 *  name Andrew
 *  width 1024
 *  height 768
 *  fullscreen no
 *
 * ConfigManager	config;
 *
 * config.load_system_config();
 * config.load_personal_config();
 *
 * config.get<string>("name") => "Andrew"
 * config.get<string>("doohickey") => ""
 * config.get<int>("width") => 1024
 * config.get<bool>("fullscreen") => false
 *
 * config.set("name", "Andromedus");
 * config.set("fullscreen", true);
 *
 * config.save_personal_config();
 *
 * The file ~/.legesmotusrc now contains:
 *  name Andromedus
 *  width 1024
 *  height 768
 *  fullscreen yes
 */

namespace LM {
	class ConfigManager {
	private:
		typedef std::map<std::string, std::string> map_type;
	
		map_type		m_options;
	
		static void		write_option(std::ostream& out, const map_type::value_type& option);
		const std::string*	lookup(const char* option_name) const;
	public:
	
		// Load and save the configuration
		bool			load(const char* filename);
		bool			load(std::istream& in);
		bool			save(const char* filename) const;
		bool			save(std::ostream& out) const;
	
		bool			load_system_config();
		bool			load_personal_config();
		bool			save_personal_config() const;
	
		static const char*	personal_config_path();
		static const char*	system_config_path();
	
		// Get the raw string value using the subscript operator:
		std::string&		operator[](const char* option_name) { return m_options[option_name]; }
		const std::string&	operator[](const char* option_name) const;
	
		// Check to see if a particular option has been set:
		bool			has(const char* option_name) const { return m_options.count(option_name); }
	
		// Unset (remove) the given option:
		void			unset(const char* option_name) { m_options.erase(option_name); }
	
		// Get the value for a particular option in the given type:
		template<class T> T	get(const char* option_name) const;
	
		// Set the value for a particular option:
		template<class T> void	set(const char* option_name, const T& option_value);

		// Clear away all options
		void			clear() { m_options.clear(); }
	};
	
	// Specializations for strings
	template<> std::string	ConfigManager::get(const char* option_name) const;
	template<> const char*	ConfigManager::get(const char* option_name) const;
	template<> void	ConfigManager::set(const char* option_name, const char* const& option_value);
	
	// Specializations for bools
	template<> bool	ConfigManager::get(const char* option_name) const;
	template<> void	ConfigManager::set(const char* option_name, const bool& option_value);

	// Specializations for uint64_t (typically used to specify time values)
	template<> uint64_t	ConfigManager::get(const char* option_name) const;
	template<> void	ConfigManager::set(const char* option_name, const uint64_t& option_value);
	
	template<class T> T	ConfigManager::get(const char* option_name) const {
		T	option_value = T();
		if (const std::string* raw_value = lookup(option_name)) {
			std::istringstream in(*raw_value);
			in >> option_value;
		}
		return option_value;
	}
	
	template<class T> void	ConfigManager::set(const char* option_name, const T& option_value) {
		std::ostringstream	out;
		out << option_value;
		m_options[option_name] = out.str();
	}
	
}

#endif
