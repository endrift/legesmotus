/*
 * MetaServer/main.cpp
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

#include "MetaServer.hpp"
#include "common/Exception.hpp"
#include "common/network.hpp"
#include "common/misc.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <cstring>

using namespace LM;
using namespace std;

extern "C" void clean_exit() {
	// TODO write
}

namespace {
	// in seconds:
	enum {
		DEFAULT_SERVER_TIMEOUT = 2700,	// 45 minutes
		DEFAULT_CONTACT_FREQUENCY = 900	// 15 minutes
	};

	void display_usage(const char* progname) {
		cout << "Usage: " << progname << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "  -d		daemonize the server (not on Windows)" << endl;
		cout << "  -u USERNAME	drop privileges to given user (not on Windows)" << endl;
		cout << "  -g GROUPNAME	drop privileges to given group (not on Windows)" << endl;
		cout << "  -f FREQUENCY	set the frequency (in seconds) at which servers" << endl;
		cout << "		should contact the meta server [default: " << int(DEFAULT_CONTACT_FREQUENCY) << "]" << endl;
		cout << "  -t TIMEOUT	set the timeout (in seconds) for servers [default: " << int(DEFAULT_SERVER_TIMEOUT) << "]" << endl;
		cout << "  -p PORTNO	set the port number to listen on [default: " << uint16_t(METASERVER_PORTNO) << "]" << endl;
		cout << "  -?, --help	display this help, and exit" << endl;
		cout << "      --version\tdisplay version information and exit" << endl;
	}

	void display_version() {
		cout << "Leges Motus Meta Server" << endl;
		cout << "A networked, 2D shooter set in zero gravity" << endl;
		cout << endl;
		cout << "Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
		cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
		cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	}
}

extern "C" int main(int argc, char* argv[]) try {
	srand(time(0));

	uint16_t		portno = METASERVER_PORTNO;
	uint32_t		contact_frequency = DEFAULT_CONTACT_FREQUENCY;
	uint32_t		server_timeout = DEFAULT_SERVER_TIMEOUT;
	bool			daemonize = false;
	const char*		username = NULL;
	const char*		groupname = NULL;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0 && argc > i+1) {
			portno = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-f") == 0 && argc > i+1) {
			contact_frequency = atol(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-t") == 0 && argc > i+1) {
			server_timeout = atol(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-u") == 0 && argc > i+1) {
			username = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-g") == 0 && argc > i+1) {
			groupname = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-d") == 0) {
			daemonize = true;
		} else if (strcmp(argv[i], "--version") == 0) {
			display_version();
			return 0;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
			display_usage(argv[0]);
			return 0;
		} else if (strncmp(argv[i], "-psn", 4) == 0) {
			//Ignore -psn for Macs
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	MetaServer		server(contact_frequency * 1000, server_timeout * 1000);

	if (!server.start(portno)) {
		cerr << "Failed to start server on port " << portno << endl;
		cerr << "Please make sure that you are not already running a service on this port." << endl;
		return 1;
	}

	if (username || groupname) {
		drop_privileges(username, groupname);
	}

	if (daemonize) {
		::daemonize();
	}

	server.run();

	return 0;

} catch (const Exception& e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
}
