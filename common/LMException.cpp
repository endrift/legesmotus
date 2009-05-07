/*
 * LMException.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include "LMException.hpp"

// See .hpp file for comments.

LMException::LMException(const char* message) : m_message(message) {
}

LMException::LMException(const std::string& message) : m_message(message) {
}

const char* LMException::what() const throw() {
	return m_message.c_str();
}
