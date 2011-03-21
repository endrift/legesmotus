#include "gui/GuiClient.hpp"
#include "client/Client.hpp"
#include "common/network.hpp"
#include "common/physics.hpp"
#include "common/Configuration.hpp"
#include "ai/AIController.hpp"
#include "ai/FuzzyLogicAI.hpp"

using namespace LM;
using namespace std;

namespace LM {
	class PathWatcher : public Widget {
	private:
		const FuzzyLogicAI* m_path_holder;
	
	public:
		PathWatcher(const FuzzyLogicAI* path_holder);
	
		virtual void draw(DrawContext* ctx) const;
	};
	
	PathWatcher::PathWatcher(const FuzzyLogicAI* path_holder) {
		m_path_holder = path_holder;
	}

	void PathWatcher::draw(DrawContext* ctx) const {
		const std::vector<SparseIntersectMap::Intersect>* path = m_path_holder->get_current_path();

		if (path == NULL) {
			return;
		}

		ctx->set_draw_color(Color(0xFF, 0, 0, 0xC0));
		std::vector<SparseIntersectMap::Intersect>::const_iterator iter = path->begin();
		if (iter == path->end()) {
			return;
		}

		SparseIntersectMap::Intersect prev = *iter;
		for (SparseIntersectMap::Intersect next = prev; iter != path->end(); ++iter) {
			SparseIntersectMap::Intersect next = *iter;

			ctx->draw_line(prev.x, prev.y, next.x, next.y);
			prev = next;
		}
	}
}

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
	game.add_extra_draw(new PathWatcher(&ai));
	game.connect(host);
	game.run();

	return 0;
}
