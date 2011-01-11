/*
 * gui/ShaderSet.hpp
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

#ifndef LM_GUI_SHADERSET_HPP
#define LM_GUI_SHADERSET_HPP

#include "DrawContext.hpp"

#include <string>

namespace LM {
	class ShaderSet {
	public:
		virtual ~ShaderSet() {}

		virtual void attach_shader(PixelShader shader) = 0;
		virtual void detach_shader(PixelShader shader) = 0;

		virtual void link() = 0;

		virtual void set_variable(const std::string& name, int x) = 0;
		virtual void set_variable(const std::string& name, int x, int y) = 0;
		virtual void set_variable(const std::string& name, int x, int y, int z) = 0;
		virtual void set_variable(const std::string& name, int x, int y, int z, int w) = 0;

		virtual void set_variable(const std::string& name, float x) = 0;
		virtual void set_variable(const std::string& name, float x, float y) = 0;
		virtual void set_variable(const std::string& name, float x, float y, float z) = 0;
		virtual void set_variable(const std::string& name, float x, float y, float z, float w) = 0;

		virtual void set_variable_1(const std::string& name, int count, int* v) = 0;
		virtual void set_variable_2(const std::string& name, int count, int* v) = 0;
		virtual void set_variable_3(const std::string& name, int count, int* v) = 0;
		virtual void set_variable_4(const std::string& name, int count, int* v) = 0;

		virtual void set_variable_1(const std::string& name, int count, float* v) = 0;
		virtual void set_variable_2(const std::string& name, int count, float* v) = 0;
		virtual void set_variable_3(const std::string& name, int count, float* v) = 0;
		virtual void set_variable_4(const std::string& name, int count, float* v) = 0;
	};
}

#endif
