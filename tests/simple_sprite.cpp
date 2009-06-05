#include "client/ClientSDL.hpp"
#include "client/GameWindow.hpp"
#include "client/Sprite.hpp"
#include "client/TiledGraphic.hpp"
#include "common/PathManager.hpp"
#include "client/Mask.hpp"
#include "client/TableBackground.hpp"
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
	Sprite mask(pman.data_path("mini_circle.png","sprites"));
	TableBackground tb(4,48);
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
	tg.set_center_x(128);
	tg.set_center_y(128);
	tg.set_width(256);
	tg.set_height(256);
	afront.set_priority(-1);
	aback.set_priority(1);
	tb.set_priority(2);
	tb.set_border_color(Color(1,1,1,0.5));
	tb.set_border_width(4);
	tb.set_cell_color(0,Color(1,0,0,0.2));
	tb.set_cell_color(1,Color(0,0,1,0.2));
	tb.set_row_height(0,24);
	tb.set_row_height(1,24);
	tb.set_row_height(2,24);
	tb.set_row_height(3,24);
	tb.set_center_y(48);
	tb.set_corner_radius(16);
	tb.set_border_collapse(true);
	Mask m(&mask,&tg);
	m.set_priority(1);
	m.set_x(250);
	m.set_y(250);
	g.add_graphic(&s);
	g.add_graphic(&aback,"f");
	g.add_graphic(&afront,"b");
	g.add_graphic(&tb);
	window->register_graphic(&g);
	window->register_graphic(&m);
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
