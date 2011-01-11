/*
 * serverscanner/JsonGenerator.hpp
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

#ifndef LM_CLIENT_JSONGENERATOR_HPP
#define LM_CLIENT_JSONGENERATOR_HPP

#include "OutputGenerator.hpp"

#include <stdint.h>
#include <ostream>
#include <string>

namespace LM {
	class JsonGenerator : public OutputGenerator {
		private:
			int m_indentation;
			bool m_needs_comma;

			void indent();

		public:
			JsonGenerator(std::ostream *outstream);

			virtual void begin();
			virtual void end();

			virtual void begin_row();
			virtual void add_cell(const std::string& column);
			virtual void end_row();

			virtual void begin_list();
			virtual void end_list();

			virtual void add_string(const std::string& str);
			virtual void add_int(int num);
			virtual void add_time(time_t sec);
			virtual void add_interval(uint64_t millis);
	};
}

#endif
