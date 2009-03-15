/*
 * Server/main.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Server.hpp"
#include "ServerSDL.hpp"
#include "common/LMException.hpp"
#include <iostream>

using namespace std;

extern "C" int main(int argc, char* argv[]) try {
	ServerSDL		server_sdl;

	Server			server;

	server.run(9009);

	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
