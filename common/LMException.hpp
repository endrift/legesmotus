/*
 * LMException.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#ifndef LM_COMMON_LMEXCEPTION_H
#define LM_COMMON_LMEXCEPTION_H

#include <exception>
#include <string>

/*
 * Represent an exception.  Contains only a message saying what happened.
 * Exceptions should only be used for truly exceptional circumstances.  Examples:
 *  The server can't listen on the port number!
 *  The client can't initialize SDL graphics
 *  Can't load the sprites
 *  etc.
 */
class LMException : std::exception {
	std::string		m_message;

public:
	explicit LMException (const char* message);
	explicit LMException (const std::string& message);
	virtual ~LMException () throw() { }

	virtual const char* what () const throw();
};


#endif
