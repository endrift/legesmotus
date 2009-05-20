/*
 * main.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GameController.hpp"
#include "ClientSDL.hpp"
#include "common/LMException.hpp"
#include "common/PathManager.hpp"
#include "common/misc.hpp"
#include "common/network.hpp"
#include "common/team.hpp"
#include <iostream>

using namespace std;

static void display_usage(const char* progname) {
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

static void display_version() {
	cout << "Leges Motus" << endl;
	cout << "A 2D team-based shooter set in zero gravity" << endl;
	cout << "Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau" << endl;
}

extern "C" int main(int argc, char* argv[]) try {
	ClientSDL		client_sdl;
	GameController*		game_controller;
	int			width = 0;
	int			height = 0;
	bool			fullscreen = false;
	char			team = 0;
	string			server = "legesmotus.beanwood.com";
	unsigned int		portno = DEFAULT_PORTNO;
	string			name = "";
	
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
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	PathManager	pathman(argv[0]);

	if (width > 0 && height > 0) {
		// Use the specified width and height
		game_controller = new GameController(pathman, width, height, fullscreen);
	} else if (fullscreen) {
		// Use optimal settings
		game_controller = new GameController(pathman);
	} else {
		// Use 1024x768 in non-fullscreen mode
		game_controller = new GameController(pathman, 1024, 768, false);
	}

	cout << "Welcome to Leges Motus." << endl;
	
	game_controller->connect_to_server(server.c_str(), portno, !name.empty() ? name : get_username(), team);
	game_controller->run();
	
	cout << "Leges Motus is now exiting." << endl;
	
	delete game_controller;
	
	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "1. If on X11, check that your $DISPLAY environment variable is set properly." << endl;
	cerr << "2. Make sure that you are running Leges Motus from the top-level source directory, OR that your $LM_DATA_DIR environment variable is set to the directory containing the game resources." << endl;
	       
	return 1;
}
