/*
 * main.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#include "GameController.hpp"
#include "common/LMException.hpp"
#include <iostream>

using namespace std;

extern "C" int main(int argc, char* argv[]) try {
	GameController		game_controller;

	cout << "Welcome to Leges Motus." << endl;
	
	game_controller.run();
	
	cout << "Leges Motus is now exiting." << endl;
	
	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
