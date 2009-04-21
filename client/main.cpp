/*
 * main.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GameController.hpp"
#include "ClientSDL.hpp"
#include "common/LMException.hpp"
#include <iostream>

using namespace std;

extern "C" int main(int argc, char* argv[]) try {
	ClientSDL		client_sdl;
	GameController*		game_controller;
	int			width = 0;
	int			height = 0;
	string			server = "legesmotus.beanwood.com";
	string			name = "";
	
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-w") == 0 && argc > i+1) {
			width = atoi(argv[i+1]);
		} else if (strcmp(argv[i], "-h") == 0 && argc > i+1) {
			height = atoi(argv[i+1]);
		} else if (strcmp(argv[i], "-s") == 0 && argc > i+1) {
			server = argv[i+1];
		} else if (strcmp(argv[i], "-n") == 0 && argc > i+1) {
			name = argv[i+1];
		}
		//cerr << "Argument " << i << ": " << argv[i] << endl;
	}
	
	if (width != 0 && height != 0) {
		game_controller = new GameController(width, height);
	} else {
		game_controller = new GameController();
	}

	cout << "Welcome to Leges Motus." << endl;
	
	game_controller->connect_to_server(server.c_str(), 9009, name);
	//game_controller->connect_to_server("haddock", 9009);
	//game_controller->connect_to_server("localhost", 9009);
	game_controller->run();
	
	cout << "Leges Motus is now exiting." << endl;
	
	delete game_controller;
	
	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
