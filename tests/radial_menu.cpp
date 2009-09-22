#include "common/PathManager.hpp"
#include "client/GameWindow.hpp"
#include "client/TextMenuItem.hpp"
#include "client/RadialMenu.hpp"
#include "client/TextManager.hpp"
#include "client/Font.hpp"
#include <iostream>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	PathManager pman(argv[0]);
	Font font(pman.data_path("JuraMedium.ttf", "fonts"), 24);
	TextManager texts(&font);

	RadialBackground* rb = new RadialBackground(1);
	rb->set_border_color(Color(1,1,1,0.8));
	rb->set_inner_radius(50.0);
	rb->set_outer_radius(50.0);
	rb->set_border_radius(10.0);
	rb->set_border_angle(8);
	rb->set_x(250);
	rb->set_y(250);
	rb->set_rotation(0);

	RadialMenu m(rb, Color(1,1,1,0.5), Color(1,1,1,1));

	m.add_item(TextMenuItem::with_manager(&texts, "A", "a", 10, 10));
	m.add_item(TextMenuItem::with_manager(&texts, "B", "b", 10, 40));

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
						cout << over->get_name() << endl;
					}
				}
				break;
			}
		}

		window->redraw();
		SDL_Delay(20);
	}
	GameWindow::destroy_instance();
	return 0;
}
