/*
 * common/misc.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_MISC_H
#define LM_COMMON_MISC_H

#include <string>


// Return the username of the current user.
// Currently only implemented in UNIX.
// On WIN32, always returns "Bill"
std::string		get_username();


#endif
