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
	Sprite s("data/sprites/blue_armless.png");
	Sprite aback("data/sprites/blue_backarm.png");
	Sprite afront("data/sprites/blue_frontarm.png");
	TiledGraphic tg("data/sprites/blue_bgtile.png");
	s.set_center_x(32);
	s.set_center_y(48);
	afront.set_center_x(46);
	afront.set_center_y(25);
	aback.set_center_x(28);
	aback.set_center_y(25);
	tg.set_start_x(250-64);
	tg.set_start_y(250-64);
	tg.set_width(128);
	tg.set_height(128);
	afront.set_priority(1);
	s.set_priority(2);
	aback.set_priority(3);
	tg.set_priority(5);
	window->register_graphic(&s);
	window->register_graphic(&aback);
	window->register_graphic(&afront);
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
		afront.set_rotation(frame / 125.0 * 360);
		aback.set_rotation(-frame / 125.0 * 360);
		s.set_x(cos(frame / 500.0 * 2*M_PI)*100+250);
		s.set_y(sin(frame / 500.0 * 2*M_PI)*100+250);
		afront.set_x(cos(frame / 500.0 * 2*M_PI)*100+250+14);
		afront.set_y(sin(frame / 500.0 * 2*M_PI)*100+250-23);
		aback.set_x(cos(frame / 500.0 * 2*M_PI)*100+250-4);
		aback.set_y(sin(frame / 500.0 * 2*M_PI)*100+250-23);
		s.set_scale_x(sin(frame / 250.0 * 2*M_PI));
		s.set_scale_y(sin(frame / 250.0 * 2*M_PI));
		afront.set_scale_x(sin(frame / 250.0 * 2*M_PI));
		afront.set_scale_y(sin(frame / 250.0 * 2*M_PI));
		aback.set_scale_x(sin(frame / 250.0 * 2*M_PI));
		aback.set_scale_y(sin(frame / 250.0 * 2*M_PI));
		++frame;
		frame %= 500;
		window->redraw();
	}
	GameWindow::destroy_instance();
	return 0;
}
