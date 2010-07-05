/*
 * serverscanner/main.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include <fstream>
#include <sstream>

using namespace LM;
using namespace std;

namespace {
	bool run_from_finder = false;

	void display_usage(const char* progname) {
		cout << "Usage: " << progname << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "  -?, --help	display this help, and exit" << endl;
		cout << "      --version\tdisplay version information and exit" << endl;
	}

	void display_version() {
		cout << "Leges Motus Server Scanner" << endl;
		cout << "Checks the Leges Motus master server." << endl;
		cout << endl;
		cout << "Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
		cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
		cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	}
}

extern "C" int main(int argc, char* argv[]) try {
	ServerScanner	scanner;
	string		file = "-";
	
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
			if (i + 1 < argc) {
				file = argv[i+1];
				++i;
			} else {
				cerr << argv[0] << ": `-f' flag requires an argument" << endl;
				display_usage(argv[0]);
				return 2;
			}
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	ostream* out;
	ofstream outf;

	if (file == "-") {
		out = &cout;
	} else {
		outf.open(file.c_str(), ios::out | ios::trunc);
		if (!outf.good()) {
			cerr << argv[0] << ": Couldn't open output file `" << file << "'" << endl;
			return 1;
		}
		out = &outf;
	}

	scanner.run(out);

	outf.close();
	
	return 0;

} catch (const Exception& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "Make sure that Leges Motus Server Scanner has been properly installed (if compiling from source, you should have run 'make install'), OR that your $LM_DATA_DIR environment variable is set to the directory containing the resources." << endl;
	return 1;
}
