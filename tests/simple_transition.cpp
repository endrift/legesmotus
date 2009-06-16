#include "client/Transition.hpp"
#include "client/TransitionManager.hpp"
#include "client/GameWindow.hpp"
#include "client/ClientSDL.hpp"
#include "client/Sprite.hpp"
#include "common/timer.hpp"
#include "common/PathManager.hpp"

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	PathManager pman(argv[0]);
	Sprite g(pman.data_path("shot.png","sprites"));
	window->register_graphic(&g);
	bool running = true;
	uint64_t start = get_ticks();
	LinearCurve cp(5.0,280.0);
	LinearCurve cs(0.5,2.0);
	LinearCurve cr(0,180);
	Transition tx(&g,&Graphic::set_x,&cp,start,2000);
	Transition ty(&g,&Graphic::set_y,&cp,start,2000);
	Transition tsx(&g,&Graphic::set_scale_x,&cs,start,2000);
	Transition tsy(&g,&Graphic::set_scale_y,&cs,start,2000);
	Transition tr(&g,&Graphic::set_rotation,&cr,start,2000);
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
