/*
 * common/PathManager.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "PathManager.hpp"
#include <cstdlib>

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
		strcpy(m_data_directory,"data");
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
	strncpy(m_data_directory,basedir,sizeof(m_data_directory));
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
