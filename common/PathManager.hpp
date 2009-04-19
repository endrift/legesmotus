/*
 * common/PathManager.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_PATHMANAGER_HPP
#define LM_COMMON_PATHMANAGER_HPP

#include <cstring>
#include <cstdio>

class PathManager {
private:
	char	m_base_directory[FILENAME_MAX];
	char	m_data_directory[FILENAME_MAX];
	static const char	m_path_separator;
	char	m_buffer[FILENAME_MAX];
public:
	PathManager(const char* argv0);
	PathManager(const char* basedir, const char* datadir);
	explicit PathManager(const PathManager& other);

	const char* data_path(const char* filename, const char* type);
	const char* exec_path(const char* filename);
	
	PathManager& operator=(const PathManager& other);
};

#endif
