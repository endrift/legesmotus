/*
 * main.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "GameController.hpp"
#include "ClientSDL.hpp"
#include "ClientConfiguration.hpp"
#include "common/Exception.hpp"
#include "common/PathManager.hpp"
#include "common/misc.hpp"
#include "common/network.hpp"
#include "common/team.hpp"
#include <iostream>
#include <sstream>

#ifdef __APPLE__
extern "C" {
#include "mac_bridge.h"
}
#elif defined(__WIN32)
#include <Windows.h>
#endif

using namespace LM;
using namespace std;

extern "C" void clean_exit() {
	SDL_Event quit_event;
	quit_event.type = SDL_QUIT;
	SDL_PushEvent(&quit_event);
}

namespace {
	bool run_from_finder = false;

	void display_usage(const char* progname) {
		cout << "Usage: " << progname << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "  -n NAME	set your player name" << endl;
		cout << "  -t red|blue	set your team" << endl;
		cout << "  -s SERVER	set the hostname of the server" << endl;
		cout << "  -p PORTNO	set the port number of the server" << endl;
		cout << "  -w WIDTH	set the screen width, in pixels" << endl;
		cout << "  -h HEIGHT	set the screen height, in pixels" << endl;
		cout << "  -f 		run the game in fullscreen" << endl;
		cout << "  -?, --help	display this help, and exit" << endl;
		cout << "      --version\tdisplay version information and exit" << endl;
	}

	void display_legalese() {
		cout << "Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
		cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
		cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	}

	void display_version() {
		cout << "Leges Motus" << endl;
		cout << "A networked, 2D shooter set in zero gravity" << endl;
		cout << endl;
		display_legalese();
	}
}

extern "C" int main(int argc, char* argv[]) try {
	ClientSDL		client_sdl;
	GameController*		game_controller;
	int			width = 0;
	int			height = 0;
	bool			fullscreen = false;
	char			team = 0;
	string			server = "";
	unsigned int		portno = DEFAULT_PORTNO;
	string			name = "";
	IPAddress		server_address;
	
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			fullscreen = true;
		} else if (strcmp(argv[i], "-w") == 0 && argc > i+1) {
			width = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-h") == 0 && argc > i+1) {
			height = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-s") == 0 && argc > i+1) {
			server = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-p") == 0 && argc > i+1) {
			portno = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-n") == 0 && argc > i+1) {
			name = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-t") == 0 && argc > i+1) {
			team = parse_team_string(argv[i+1]);
			if (!is_valid_team(team)) {
				cerr << argv[0] << ": Unrecognized team `" << argv[i+1] << "'" << endl;
				display_usage(argv[0]);
				return 2;
			}
			++i;
		} else if (strcmp(argv[i], "--version") == 0) {
			display_version();
			return 0;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
			display_usage(argv[0]);
			return 0;
		} else if (strncmp(argv[i], "-psn", 4) == 0) {
			run_from_finder = true;
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	PathManager	pathman(argv[0]);

	bool restart = true;
	while(restart) {
		if (!server.empty() && !resolve_hostname(server_address, server.c_str(), portno)) {
			cerr << "Hostname of requested server not found: " << server << endl;
			return 1;
		}

		if (has_terminal_output()) {
			cout << "Welcome to Leges Motus." << endl;
			display_legalese();
		}

		ClientConfiguration config;
		
		if (width > 0 && height > 0) {
			// Use the specified width and height
			game_controller = new GameController(pathman, &config, width, height, fullscreen);
		} else if (fullscreen) {
			// Use optimal settings
			game_controller = new GameController(pathman, &config);
		} else {
			width = config.get_int_value("screen_width");
			height = config.get_int_value("screen_height");
			fullscreen = config.get_bool_value("fullscreen");
			if (width == -1) {
				width = 800;
			}
			if (height == -1) {
				height = 600;
			}
			// Use the default from the config
			game_controller = new GameController(pathman, &config, width, height, fullscreen);
		}

		if (!name.empty()) {
			config.set_string_value("name", name);
		} else {
			if (config.get_string_value("name") == "" || config.get_string_value("name") == "Unnamed") {
				config.set_string_value("name", get_username());
			}
			name = config.get_string_value("name");
		}

		game_controller->set_player_name(!name.empty() ? name : get_username());
		if (!server.empty()) {
			game_controller->connect_to_server(server_address, team);
		}

		game_controller->run();
		
		if (game_controller->wants_restart()) {
			// Allow the resolution to be re-specified by the config.
			width = 0;
			height = 0;
			fullscreen = false;
			
			// Reconnect to the server we were on.
			server = game_controller->get_server_address();
			size_t portdelim = server.find(":");
			if (portdelim != string::npos) {
				portno = atoi(server.substr(portdelim+1).c_str());
				server = server.substr(0, portdelim);
			}
			name = game_controller->get_player_name();
			game_controller->disconnect();
		} else {
			restart = false;
		}
		
		delete game_controller;
	}
	
	if (has_terminal_output()) {
		cout << "Leges Motus is now exiting.  Thanks for playing!" << endl;
	}
	
	return 0;

} catch (const Exception& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "1. If on X11, check that your $DISPLAY environment variable is set properly." << endl;
	cerr << "2. Make sure that Leges Motus has been properly installed (if compiling from source, you should have run 'make install'), OR that your $LM_DATA_DIR environment variable is set to the directory containing the game resources." << endl;

	#ifdef __APPLE__
	if (run_from_finder) {
		toplevel_exception(e.what());
	}
	#elif defined(__WIN32)
	stringstream s;
	s << e.what() << endl;
	s << "Please ensure that you are running Leges Motus from the same directory as the game resources" << flush;
	MessageBox(NULL, s.str().c_str(), NULL, MB_OK|MB_ICONERROR);
	#endif

	return 1;
}
