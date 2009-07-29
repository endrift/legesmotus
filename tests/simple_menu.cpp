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
	Text *currtext;
	TextMenuItem *curritem;

	currtext = texts.place_string("First", 10, 10);
	curritem = new TextMenuItem(currtext->clone(), "first");
	texts.remove_string(currtext);
	m.add_item(curritem);

	currtext = texts.place_string("Second", 10, 40);
	curritem = new TextMenuItem(currtext->clone(), "second");
	texts.remove_string(currtext);
	m.add_item(curritem);

	currtext = texts.place_string("Unclickable", 10, 70);
	curritem = new TextMenuItem(currtext->clone(), "static", MenuItem::STATIC);
	texts.remove_string(currtext);
	m.add_item(curritem);

	currtext = texts.place_string("Disabled", 10, 100);
	curritem = new TextMenuItem(currtext->clone(), "disabled", MenuItem::DISABLED);
	texts.remove_string(currtext);
	m.add_item(curritem);

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
