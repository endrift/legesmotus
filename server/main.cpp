/*
 * Server/main.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "Server.hpp"
#include "ServerSDL.hpp"
#include "common/LMException.hpp"
#include "common/network.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

static void display_usage(const char* progname) {
	cout << "Usage: " << progname << " [OPTION]" << endl;
	cout << "Options:" << endl;
	cout << "  -p PORTNO	set the port number to listen on" << endl;
	cout << "  -P PASSWORD	set the admin password" << endl;
	cout << "  -m MAPNAME	set the map name" << endl;
	cout << "  -?, --help	display this help, and exit" << endl;
	cout << "      --version\tdisplay version information and exit" << endl;
}

static void display_version() {
	cout << "Leges Motus Server" << endl;
	cout << "A 2D team-based shooter set in zero gravity" << endl;
	cout << "Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau" << endl;
}

extern "C" int main(int argc, char* argv[]) try {
	ServerSDL		server_sdl;

	srand(time(0));

	string			password;
	string			map_name("alpha1"); // XXX: what should the default map be?  should there be a default map?
	unsigned int		portno = DEFAULT_PORTNO;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0 && argc > i+1) {
			portno = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-P") == 0 && argc > i+1) {
			password = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-m") == 0 && argc > i+1) {
			map_name = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "--version") == 0) {
			display_version();
			return 0;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
			display_usage(argv[0]);
			return 0;
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	Server			server;

	server.set_password(password.c_str());
	server.run(portno, map_name.c_str());

	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
