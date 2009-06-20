#include "common/ConfigManager.hpp"
#include "common/misc.hpp"
#include <iostream>
#include <string>

using namespace LM;
using namespace std;

int main () {
	ConfigManager		config;

	if (!config.load_system_config()) {
		cerr << "Failed to load system config." << endl;
	}
	if (!config.load_personal_config()) {
		cerr << "Failed to load personal config." << endl;
	}

	cout << "name=" << config.get<string>("name") << endl;
	cout << "width=" << config.get<int>("width") << endl;
	cout << "height=" << config.get<int>("height") << endl;
	cout << "fullscreen=" << config.get<bool>("fullscreen") << endl;

	if (!config.has("name")) {
		config.set("name", get_username());
	}
	if (!config.has("fullscreen")) {
		config.set("fullscreen", true);
	}

	if (!config.save_personal_config()) {
		cerr << "Failed to save personal config." << endl;
	}

	return 0;
}
