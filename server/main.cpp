/*
 * Server/main.cpp
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

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "common/Exception.hpp"
#include "common/network.hpp"
#include "common/PathManager.hpp"
#include "common/StringTokenizer.hpp"
#include "common/misc.hpp"
#include "common/timer.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <memory>
#include <signal.h>

using namespace LM;
using namespace std;

namespace {
	auto_ptr<Server>		server;

	void display_usage(const char* progname) {
		cout << "Usage: " << progname << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "  -c CONFFILE	load the given configuration file" << endl;
		cout << "  -o OPT=VALUE	set the configuration option named OPT to VALUE" << endl;
		cout << "  -m MAPNAME	set the map name [default: alpha1]" << endl;
		cout << "  -P PASSWORD	set the admin password [default: DISABLED]" << endl;
		cout << "  -p PORTNO	set the port number to listen on [default: " << DEFAULT_PORTNO << "]" << endl;
		cout << "  -i ADDRESS	address of interface to listen on [default: all interfaces]" << endl;
		cout << "  -d		daemonize the server (not on Windows)" << endl;
		cout << "  -u USERNAME	drop privileges to given user (only super user may use) (not on Windows)" << endl;
		cout << "  -g GROUPNAME	drop privileges to given group (only super user may use) (not on Windows)" << endl;
		cout << "  -l		(local server) do not register with the meta server" << endl;
		cout << "  -?, --help	display this help, and exit" << endl;
		cout << "      --version\tdisplay version information and exit" << endl;
	}

	void display_version() {
		cout << "Leges Motus Server " << Server::SERVER_VERSION << endl;
		cout << "A networked, 2D shooter set in zero gravity" << endl;
		cout << endl;
		cout << "Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
		cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
		cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	}

	void	graceful_termination_handler (int)
	{
		server->stop();
	}

	void	restart_server_handler (int)
	{
		server->restart();
	}

	void	init_signals () {
	#ifdef __WIN32
		signal(SIGINT, graceful_termination_handler);
		signal(SIGTERM, graceful_termination_handler);
	#else
		struct sigaction	siginfo;

		// Block all these signals, and only unblock them when the server is ready to handle them (typically when it's waiting on a select)
		sigemptyset(&siginfo.sa_mask);
		sigaddset(&siginfo.sa_mask, SIGHUP);
		sigaddset(&siginfo.sa_mask, SIGINT);
		sigaddset(&siginfo.sa_mask, SIGTERM);
		sigprocmask(SIG_BLOCK, &siginfo.sa_mask, NULL);

		// Ignore SIGCHLD
		siginfo.sa_flags = 0;
		siginfo.sa_handler = SIG_IGN;
		sigaction(SIGCHLD, &siginfo, NULL);

		// Restart the server on a SIGHUP
		siginfo.sa_flags = 0;
		siginfo.sa_handler = restart_server_handler;
		sigaction(SIGHUP, &siginfo, NULL);

		// Gracefully terminate on a SIGINT or a SIGTERM
		siginfo.sa_flags = 0;
		siginfo.sa_handler = graceful_termination_handler;
		sigaction(SIGINT, &siginfo, NULL);
		sigaction(SIGTERM, &siginfo, NULL);
	#endif
	}
}

extern "C" void clean_exit() {
	if (server.get()) {
		server->stop();
	}
}

extern "C" int main(int argc, char* argv[]) try {
	srand(time(0));
	get_ticks(); // Initialize the tick counter to 0 (needed because the INFO packet contains the server uptime)

	ServerConfig		config;
	const char*		username = NULL;
	const char*		groupname = NULL;
	bool			daemonize = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0 && argc > i+1) {
			if (!config.load(argv[i+1])) {
				cerr << argv[i+1] << ": failed to load configuration file" << endl;
				return 1;
			}
			++i;
		} else if (strcmp(argv[i], "-o") == 0 && argc > i+1) {
			// Note: saving argv[i+1] to arg is necessary to work around some weird gcc 3.3.4 bug
			const char*	arg = argv[i+1];
			string		option_name;
			string		option_value;
			StringTokenizer(arg, '=', 2) >> option_name >> option_value;
			config.set(option_name.c_str(), option_value.c_str());
			++i;
		} else if (strcmp(argv[i], "-i") == 0 && argc > i+1) {
			config.set("interface", argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-p") == 0 && argc > i+1) {
			config.set("portno", atoi(argv[i+1]));
			++i;
		} else if (strcmp(argv[i], "-P") == 0 && argc > i+1) {
			config.set("password", argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-m") == 0 && argc > i+1) {
			config.set("map", argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-l") == 0) {
			config.set("register_server", false);
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

	PathManager		path_manager(argv[0]);

	server.reset(new Server(config, path_manager));

	server->start();

	if (username || groupname) {
		drop_privileges(username, groupname);
	}

	if (daemonize) {
		::daemonize();
	}

	init_signals();

	server->run();

	return 0;

} catch (const Exception& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "1. Make sure that Leges Motus has been properly installed (if compiling from source, you should have run 'make install'), OR that your $LM_DATA_DIR environment variable is set to the directory containing the game resources." << endl;
	cerr << "2. Make sure that you are not already running the server." << endl;
	return 1;
}
