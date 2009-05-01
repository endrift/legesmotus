#include "client/ClientSDL.hpp"
#include "client/GameWindow.hpp"
#include "client/Sprite.hpp"
#include "client/TiledGraphic.hpp"
#include "common/PathManager.hpp"
#include <cmath>

using namespace std;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	PathManager pman(argv[0]);
	Sprite s(pman.data_path("blue_armless.png","sprites"));
	Sprite aback(pman.data_path("blue_backarm.png","sprites"));
	Sprite afront(pman.data_path("blue_frontarm.png","sprites"));
	TiledGraphic tg(pman.data_path("metal_bgtile.png","sprites"));
	GraphicGroup g;
	s.set_center_x(32);
	s.set_center_y(48);
	afront.set_center_x(46);
	afront.set_center_y(30);
	afront.set_x(13);
	afront.set_y(-18);
	aback.set_center_x(27);
	aback.set_center_y(29);
	aback.set_x(-2);
	aback.set_y(-18);
	tg.set_start_x(250-64);
	tg.set_start_y(250-64);
	tg.set_width(128);
	tg.set_height(128);
	afront.set_priority(-1);
	aback.set_priority(1);
	tg.set_priority(1);
	g.add_graphic(&s);
	g.add_graphic(&aback,"f");
	g.add_graphic(&afront,"b");
	window->register_graphic(&g);
	window->register_graphic(&tg);
	bool running = true;
	int frame = 0;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = 0;
				} break;
			}
		}

		g.set_rotation(-frame / 400.0 * 360);
		g.get_graphic("f")->set_rotation(frame / 80.0 * 360);
		g.get_graphic("b")->set_rotation(-frame / 80.0 * 360);
		g.set_x(cos(frame / 400.0 * 2*M_PI)*100+250);
		g.set_y(sin(frame / 400.0 * 2*M_PI)*100+250);
		g.set_scale_x((sin(frame / 200.0 * 2*M_PI)+1)/2.0);
		g.set_scale_y((sin(frame / 200.0 * 2*M_PI)+1)/2.0);
		++frame;
		frame %= 400;
		window->redraw();
		SDL_Delay(20);
	}
	GameWindow::destroy_instance();
	return 0;
}
