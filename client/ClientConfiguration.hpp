/*
 * client/ClientConfiguration.hpp
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

#include "common/ConfigManager.hpp"

#ifndef LM_CLIENT_CLIENTCONFIGURATION_HPP
#define LM_CLIENT_CLIENTCONFIGURATION_HPP

namespace LM {
	class ClientConfiguration {
	private:
		ConfigManager m_config_manager;
		std::map<std::string, std::string> m_string_vals;
		std::map<std::string, std::string> m_string_defaults;
		std::map<std::string, int> m_int_vals;
		std::map<std::string, int> m_int_defaults;
		std::map<std::string, bool> m_bool_vals;
		std::map<std::string, bool> m_bool_defaults;
	
	public:
		ClientConfiguration();
		~ClientConfiguration();
		
		void		reload_config();
		void		save_config();
		void		reset_to_defaults();
		std::string	get_string_value(std::string name);
		int		get_int_value(std::string name);
		bool		get_bool_value(std::string name);
		void		set_string_value(std::string name, std::string value);
		void		set_int_value(std::string name, int value);
		void		set_bool_value(std::string name, bool value);
	};
	
}

#endif
