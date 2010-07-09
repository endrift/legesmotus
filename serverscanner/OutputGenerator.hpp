/*
 * serverscanner/OutputGenerator.hpp
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

#ifndef LM_CLIENT_OUTPUTGENERATOR_HPP
#define LM_CLIENT_OUTPUTGENERATOR_HPP

#include <stdint.h>
#include <ostream>
#include <string>
#include <map>

namespace LM {
	class OutputGenerator {
		private:
			std::ostream* m_out;
			std::map<std::string, std::string> m_col_mapping;

		protected:
			std::ostream& out();

		public:
			OutputGenerator(std::ostream *outstream);
			virtual ~OutputGenerator() {};

			virtual void begin() = 0;
			virtual void end() = 0;

			void add_column(const std::string& shortname, const std::string& longname);
			const std::string& get_column(const std::string& shortname);

			virtual void begin_row() = 0;
			virtual void add_cell(const std::string& column) = 0;
			virtual void end_row() = 0;

			virtual void begin_list() = 0;
			virtual void end_list() = 0;

			virtual void add_string(const std::string& str) = 0;
			virtual void add_int(int num) = 0;
			virtual void add_time(time_t sec) = 0;
			virtual void add_interval(uint64_t millis) = 0;
	};
}

#endif
