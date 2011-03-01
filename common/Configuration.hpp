/*
 * common/Configuration.hpp
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

#ifndef LM_COMMON_CONFIGURATION_HPP
#define LM_COMMON_CONFIGURATION_HPP

#include "common/Iterator.hpp"

#include <string>

#include "thirdparty/simpleini/SimpleIni.h"

namespace LM {
	class Configuration {
	private:
		CSimpleIniA* m_local;
		CSimpleIniA* m_global;

	public:
		static const std::string& local_dir();
		static const std::string& global_dir();
		static bool key_exists(CSimpleIniA* dict, const char* section, const char* key);

		Configuration();
		Configuration(const std::string& filename);
		~Configuration();
		
		bool local_key_exists(const char* section, const char* key) const;
		bool global_key_exists(const char* section, const char* key) const;
		bool key_exists(const char* section, const char* key) const;

		ConstIterator<std::pair<const char*, const char*> > get_section(const char* section) const;
		const char* get_string(const char* section, const char* key, const char* dflt = NULL) const;
		int get_int(const char* section, const char* key, int dflt = NULL) const;
		bool get_bool(const char* section, const char* key, bool dflt = NULL) const;
		float get_float(const char* section, const char* key, float dflt = NULL) const;

		void set_string(const char* section, const char* key, const char* val);
		void set_int(const char* section, const char* key, int val);
		void set_bool(const char* section, const char* key, bool val);
		void set_float(const char* section, const char* key, float val);

		bool save(const std::string& filename) const;
	};
}

#endif
