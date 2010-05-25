#include "client/Font.hpp"
#include "client/GameWindow.hpp"
#include "client/TextManager.hpp"
#include "client/TextInput.hpp"
#include "client/TableBackground.hpp"
#include "common/PathManager.hpp"
#include "client/Curve.hpp"
#include <string>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 75, 24, false);
	PathManager pman(argv[0]);
	Font font(pman.data_path("JuraMedium.ttf","fonts"),12);
	ConstantCurve curve(0, 1);
	TextManager texts(&font,window);
	TextInput inp(&texts, 10, 50, 100);
	texts.set_active_color(1.0,1.0,1.0);
	texts.set_shadow(true);
	texts.set_shadow_convolve(&curve, 4);
	texts.set_shadow_offset(0.0,-1.0);
	texts.set_shadow_color(0.0,1.0,0.0);
	TableBackground *tb = new TableBackground(1, 0);
	tb->set_priority(10);
	tb->set_border_color(Color(1,1,1,0.5));
	tb->set_border_width(1);
	tb->set_cell_color(0,Color(1,0,0,0.2));
	inp.set_window(window);
	inp.set_crop_width(285);
	inp.set_prefix("TEST> ");
	inp.set_background_padding(5);
	inp.set_background_scale(true);
	inp.set_background(tb);
	bool running = true;
	SDL_EnableUNICODE(1);
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
				} else {
					inp.keyboard_event(e.key);
				}
				break;
			}
		}
		window->redraw();
		SDL_Delay(100);
	}
	return 0;
}
