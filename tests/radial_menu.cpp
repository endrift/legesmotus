#include "common/PathManager.hpp"
#include "client/GameWindow.hpp"
#include "client/GraphicMenuItem.hpp"
#include "client/RadialMenu.hpp"
#include "client/Sprite.hpp"
#include <iostream>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	PathManager pman(argv[0]);
	Sprite ch(pman.data_path("crosshairs.png","sprites"));

	RadialBackground* rb = new RadialBackground(1);
	rb->set_border_color(Color(1,1,1,0.2));
	rb->set_inner_radius(80.0);
	rb->set_outer_radius(30.0);
	rb->set_border_radius(2.0);
	rb->set_border_angle(1);
	rb->set_x(250);
	rb->set_y(250);
	rb->set_rotation(45);

	RadialMenu m(rb, Color(1,1,1,0.5), Color(1,1,1,1));

	GraphicMenuItem *cur_item;
	cur_item = new GraphicMenuItem(new Sprite(pman.data_path("gun_noshot.png", "sprites")), "gun");
	cur_item->set_hover_scale(2.0);
	m.add_item(cur_item);
	cur_item = new GraphicMenuItem(new Sprite(pman.data_path("mgun_noshot.png", "sprites")), "mgun");
	cur_item->set_hover_scale(2.0);
	m.add_item(cur_item);
	cur_item = new GraphicMenuItem(new Sprite(pman.data_path("cannon_front.png", "sprites")), "cannon", MenuItem::DISABLED);
	cur_item->set_hover_scale(2.0);
	m.add_item(cur_item);

	window->register_graphic(m.get_graphic_group());
	window->register_graphic(&ch);

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
				ch.set_x(e.motion.x);
				ch.set_y(e.motion.y);
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
