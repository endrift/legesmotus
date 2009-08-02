#include "common/PathManager.hpp"
#include "client/GameWindow.hpp"
#include "client/TextMenuItem.hpp"
#include "client/ArbitraryMenu.hpp"
#include "client/TextManager.hpp"
#include "client/Font.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	SDL_ShowCursor(SDL_TRUE);

	PathManager pman(argv[0]);
	ArbitraryMenu m;
	Font font(pman.data_path("JuraMedium.ttf", "fonts"),24);
	TextManager texts(&font);
	ConstantCurve curve(0, 1);
	texts.set_shadow(true);
	texts.set_shadow_offset(2, 2);
	texts.set_shadow_convolve(&curve, 8, 1);

	m.add_item(TextMenuItem::with_manager(&texts, "First", "first", 10, 10));
	m.add_item(TextMenuItem::with_manager(&texts, "Second", "second", 10, 40));
	m.add_item(TextMenuItem::with_manager(&texts, "Unclickable", "static", 10, 70, MenuItem::STATIC));
	m.add_item(TextMenuItem::with_manager(&texts, "Disabled", "disabled", 10, 100, MenuItem::DISABLED));

	window->register_graphic(m.get_graphic_group());

	bool running = true;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				default: break;
				}
				break;
			case SDL_MOUSEMOTION:
				m.mouse_motion_event(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP: {
					MenuItem* over = m.mouse_button_event(e.button);
					if (over != NULL) {
						cout << over->get_value() << endl;
					}
				}
				break;
			}
		}

		window->redraw();
		SDL_Delay(100);
	}

	return 0;
}
