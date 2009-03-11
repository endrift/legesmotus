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

int main(int argc, const char* argv[]) try {
	GameController		game_controller;

	cout << "Hi." << endl;

	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
}
