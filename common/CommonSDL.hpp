/*
 * common/CommonSDL.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_COMMONSDL_HPP
#define LM_COMMON_COMMONSDL_HPP

/*
 * The purpose of this class is to:
 *  - initialize all the commonly-used SDL libraries in the constructor
 *  - shutdown the libraries in the destructor
 */

class CommonSDL {
public:
	CommonSDL();
	~CommonSDL();
};

#endif
