#include "client/Transition.hpp"
#include "client/TransitionManager.hpp"
#include "client/GameWindow.hpp"
#include "client/ClientSDL.hpp"
#include "client/Sprite.hpp"
#include "common/timer.hpp"
#include "common/PathManager.hpp"
#include "common/math.hpp"

using namespace LM;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	PathManager pman(argv[0]);
	Sprite g(pman.data_path("shot.png", "sprites"));
	window->register_graphic(&g);
	bool running = true;
	uint64_t start = get_ticks();
	SinusoidalCurve cx(10.0, 290.0, 1.0, M_PI*0.5);
	SinusoidalCurve cy(10.0, 290.0, 1.0, 0.0);
	SinusoidalCurve cs(1.0, 2.0, 2.0, 0.0);
	LinearCurve cr(0, 360);
	Transition tx(&g, &Graphic::set_x, &cx,start,2000);
	Transition ty(&g, &Graphic::set_y, &cy,start,2000);
	Transition tsx(&g, &Graphic::set_scale_x, &cs,start,2000);
	Transition tsy(&g, &Graphic::set_scale_y, &cs,start,2000);
	Transition tr(&g, &Graphic::set_rotation, &cr,start,2000);
	TransitionManager tm;
	tm.add_transition(&tx,true);
	tm.add_transition(&ty,true);
	tm.add_transition(&tsx,true);
	tm.add_transition(&tsy,true);
	tm.add_transition(&tr,true);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				} break;
			}
		}
		tm.update(get_ticks());
		window->redraw();
	}
	
	GameWindow::destroy_instance();
	return 0;
}
