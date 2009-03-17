#include "client/ClientSDL.hpp"
#include "client/GameWindow.hpp"
#include "client/Sprite.hpp"
#include <cmath>

using namespace std;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(500, 500, 32, false);
	Sprite s("data/sprites/blue_full.png");
	s.set_center_x(24);
	s.set_center_y(48);
	window->register_sprite(&s);
	bool running = true;
	int frame = 0;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = 0;
				} break;
			}
		}

		s.set_rotation(frame / 5000.0 * 360);
		s.set_x(cos(frame / 5000.0 * 2*M_PI)*100+250);
		s.set_y(sin(frame / 5000.0 * 2*M_PI)*100+250);
		++frame;
		frame %= 5000;
		window->redraw();
	}
	GameWindow::destroy_instance();
	return 0;
}
