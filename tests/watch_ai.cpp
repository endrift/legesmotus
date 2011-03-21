#include "gui/GuiClient.hpp"
#include "client/Client.hpp"
#include "common/network.hpp"
#include "common/Configuration.hpp"
#include "ai/AIController.hpp"
#include "ai/FuzzyLogicAI.hpp"

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GuiClient game;
	Configuration config("ai.ini");
	FuzzyLogicAI ai(&config);
	AIController controller(&ai);
	game.set_controller(&controller);
	game.set_config(&config);
	IPAddress host;
	const char* server = config.get_string("Network", "server", "endrift.com:16876");
	resolve_hostname(host, server);
	game.connect(host);
	game.run();

	return 0;
}
