/*
 * gui/GLESProgram.cpp
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

#include "GLESProgram.hpp"

using namespace LM;
using namespace std;

GLESProgram::GLESProgram() {
	m_program = glCreateProgram();
}

GLESProgram::~GLESProgram() {
	LM_gl(DeleteProgram, (m_program));
}

GLuint GLESProgram::program_number() {
	return m_program;
}

GLuint GLESProgram::get_uniform(const std::string& name) {
	return glGetUniformLocation(m_program, name.c_str());
}

void GLESProgram::attach_shader(PixelShader shader) {
	LM_gl(AttachShader, (m_program, shader));
}

void GLESProgram::detach_shader(PixelShader shader) {
	LM_gl(DetachShader, (m_program, shader));
}

void GLESProgram::link() {
	LM_gl(LinkProgram, (m_program));
	GLint linked = 0;
	LM_gl(GetProgramiv, (m_program, LM_GL(COMPILE_STATUS), &linked));
	if (!linked) {
		LM_gl(GetProgramiv, (m_program, LM_GL(INFO_LOG_LENGTH), &linked));
		char* log = new char[linked];
		LM_gl(GetProgramInfoLog, (m_program, linked, &linked, log));
		WARN("Shader linking failed: " << log);
		delete[] log;
	}
}

void GLESProgram::set_variable(const std::string& name, int x) {
	LM_gl(Uniform1i, (get_uniform(name), x));
}

void GLESProgram::set_variable(const std::string& name, int x, int y) {
	LM_gl(Uniform2i, (get_uniform(name), x, y));
}

void GLESProgram::set_variable(const std::string& name, int x, int y, int z) {
	LM_gl(Uniform3i, (get_uniform(name), x, y, z));
}

void GLESProgram::set_variable(const std::string& name, int x, int y, int z, int w) {
	LM_gl(Uniform4i, (get_uniform(name), x, y, z, w));
}

void GLESProgram::set_variable(const std::string& name, float x) {
	LM_gl(Uniform1f, (get_uniform(name), x));
}

void GLESProgram::set_variable(const std::string& name, float x, float y) {
	LM_gl(Uniform2f, (get_uniform(name), x, y));
}

void GLESProgram::set_variable(const std::string& name, float x, float y, float z) {
	LM_gl(Uniform3f, (get_uniform(name), x, y, z));
}

void GLESProgram::set_variable(const std::string& name, float x, float y, float z, float w) {
	LM_gl(Uniform4f, (get_uniform(name), x, y, z, w));
}

void GLESProgram::set_variable_1(const std::string& name, int count, int* v) {
	LM_gl(Uniform1iv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_2(const std::string& name, int count, int* v) {
	LM_gl(Uniform2iv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_3(const std::string& name, int count, int* v) {
	LM_gl(Uniform3iv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_4(const std::string& name, int count, int* v) {
	LM_gl(Uniform4iv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_1(const std::string& name, int count, float* v) {
	LM_gl(Uniform1fv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_2(const std::string& name, int count, float* v) {
	LM_gl(Uniform2fv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_3(const std::string& name, int count, float* v) {
	LM_gl(Uniform3fv, (get_uniform(name), count, v));
}

void GLESProgram::set_variable_4(const std::string& name, int count, float* v) {
	LM_gl(Uniform4fv, (get_uniform(name), count, v));
}
