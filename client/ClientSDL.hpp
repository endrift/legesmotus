/*
 * client/ClientSDL.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_CLIENTSDL_HPP
#define LM_CLIENT_CLIENTSDL_HPP

/*
 * The purpose of this class is to:
 *  - initialize the SDL library in the constructor
 *  - shutdown the library in the destructor
 *
 * You should declare an instance of this class inside main().
 * It will take care of proper initialization and de-initialization quite nicely.
 */

class ClientSDL {
public:
	ClientSDL();
	~ClientSDL();

};

#endif
