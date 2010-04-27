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

#include "ServerScanner.hpp"
#include "common/Exception.hpp"
#include "common/PathManager.hpp"
#include "common/misc.hpp"
#include "common/network.hpp"
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
}

namespace {
	bool run_from_finder = false;

	void display_usage(const char* progname) {
		cout << "Usage: " << progname << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "  -?, --help	display this help, and exit" << endl;
		cout << "      --version\tdisplay version information and exit" << endl;
	}

	void display_legalese() {
		cout << "Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
		cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
		cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	}

	void display_version() {
		cout << "Leges Motus Server Scanner" << endl;
		cout << "Checks the Leges Motus master server." << endl;
		cout << endl;
		display_legalese();
	}
}

extern "C" int main(int argc, char* argv[]) try {
	ServerScanner*		game_controller;
	string			file = "";
	
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--version") == 0) {
			display_version();
			return 0;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
			display_usage(argv[0]);
			return 0;
		} else if (strncmp(argv[i], "-psn", 4) == 0) {
			run_from_finder = true;
		} else if (strncmp(argv[i], "-f", 4) == 0) {
			file = argv[i+1];
			++i;
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	PathManager	pathman(argv[0]);

	bool restart = true;
	while(restart) {

		if (has_terminal_output()) {
			cout << "Leges Motus Server Scanner" << endl;
			display_legalese();
		}
		
		game_controller = new ServerScanner();
		game_controller->run(file);
		
		delete game_controller;
	}
	
	if (has_terminal_output()) {
		cout << "Leges Motus Server Scanner is now exiting." << endl;
	}
	
	return 0;

} catch (const Exception& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "1. If on X11, check that your $DISPLAY environment variable is set properly." << endl;
	cerr << "2. Make sure that Leges Motus Server Scanner has been properly installed (if compiling from source, you should have run 'make install'), OR that your $LM_DATA_DIR environment variable is set to the directory containing the resources." << endl;

	#ifdef __APPLE__
	if (run_from_finder) {
		toplevel_exception(e.what());
	}
	#elif defined(__WIN32)
	stringstream s;
	s << e.what() << endl;
	s << "Please ensure that you are running the Server Scanner from the same directory as the game resources" << flush;
	MessageBox(NULL, s.str().c_str(), NULL, MB_OK|MB_ICONERROR);
	#endif

	return 1;
}
