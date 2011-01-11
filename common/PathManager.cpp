/*
 * common/PathManager.cpp
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

#include "PathManager.hpp"
#include <cstdlib>

using namespace LM;

#ifdef __WIN32
#include <direct.h>
#define getcwd _getcwd
const char PathManager::m_path_separator = '\\';
#else
#include <unistd.h>
const char PathManager::m_path_separator = '/';
#endif

PathManager::PathManager(const char* argv0) {
	(void)(argv0); //To be used later?
	const char* loc = getenv("LM_DATA_DIR");
	if (loc == NULL) {
		strncpy(m_data_directory,LM_DATA_DIR,sizeof(m_data_directory));
	} else {
		strncpy(m_data_directory,loc,sizeof(m_data_directory));
		m_data_directory[sizeof(m_data_directory)-1] = '\0';
	}
	loc = getenv("LM_EXEC_DIR");
	if (loc == NULL) {
		strcpy(m_base_directory,".");
	} else {
		strncpy(m_base_directory,loc,sizeof(m_base_directory));
		m_base_directory[sizeof(m_base_directory)-1] = '\0';
	}
}

PathManager::PathManager(const char* basedir, const char* datadir) {
	strncpy(m_base_directory,basedir,sizeof(m_base_directory));
	m_base_directory[sizeof(m_base_directory)-1] = '\0';
	strncpy(m_data_directory,datadir,sizeof(m_data_directory));
	m_data_directory[sizeof(m_data_directory)-1] = '\0';
}

PathManager::PathManager(const PathManager& other) {
	memcpy(m_base_directory,other.m_base_directory,sizeof(m_base_directory));
	memcpy(m_data_directory,other.m_data_directory,sizeof(m_data_directory));
}

const char* PathManager::data_path(const char* filename, const char* type) {
	snprintf(m_buffer,sizeof(m_buffer),"%s%c%s%c%s",m_data_directory,m_path_separator,type,m_path_separator,filename);
	m_buffer[sizeof(m_buffer)-1] = '\0';
	return m_buffer;
}

const char* PathManager::exec_path(const char* filename) {
	snprintf(m_buffer,sizeof(m_buffer),"%s%c%s",m_base_directory,m_path_separator,filename);
	m_buffer[sizeof(m_buffer)-1] = '\0';
	return m_buffer;
}

PathManager& PathManager::operator=(const PathManager& other) {
	memmove(m_base_directory,other.m_base_directory,sizeof(m_base_directory));
	memmove(m_data_directory,other.m_data_directory,sizeof(m_data_directory));
	return *this;
}
