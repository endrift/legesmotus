#include "client/ClientSDL.hpp"
#include "client/GameWindow.hpp"
#include "client/Sprite.hpp"
#include "client/TiledGraphic.hpp"
#include <cmath>

using namespace std;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	Sprite s("data/sprites/blue_full.png");
	TiledGraphic tg("data/sprites/blue_bgtile.png");
	s.set_center_x(24);
	s.set_center_y(48);
	tg.set_start_x(250-64);
	tg.set_start_y(250-64);
	tg.set_width(128);
	tg.set_height(128);
	tg.set_priority(1);
	window->register_graphic(&s);
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

		s.set_rotation(frame / 500.0 * 360);
		s.set_x(cos(frame / 500.0 * 2*M_PI)*100+250);
		s.set_y(sin(frame / 500.0 * 2*M_PI)*100+250);
		++frame;
		frame %= 500;
		window->redraw();
	}
	GameWindow::destroy_instance();
	return 0;
}
