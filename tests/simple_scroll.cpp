#include "common/timer.hpp"
#include "common/PathManager.hpp"
#include "client/GameWindow.hpp"
#include "client/ScrollArea.hpp"
#include "client/ScrollBar.hpp"
#include "client/Sprite.hpp"

using namespace LM;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300,300,24,false);
	ScrollBar vbar;
	vbar.set_x(280);
	vbar.set_y(150);
	vbar.set_height(240);
	vbar.set_section_color(ScrollBar::BUTTONS, Color(0.5,0.5,0.5));
	vbar.set_section_color(ScrollBar::TRACK, Color(0.2,0.2,0.2));
	vbar.set_section_color(ScrollBar::TRACKER, Color(0.4,0.4,0.4));
	ScrollBar hbar;
	hbar.set_x(150);
	hbar.set_y(280);
	hbar.set_height(240);
	hbar.set_section_color(ScrollBar::BUTTONS, Color(0.5,0.5,0.5));
	hbar.set_section_color(ScrollBar::TRACK, Color(0.2,0.2,0.2));
	hbar.set_section_color(ScrollBar::TRACKER, Color(0.4,0.4,0.4));
	ScrollArea area(250,250,300,300,&hbar,&vbar);
	area.set_x(150);
	area.set_y(150);
	area.set_center_x(125);
	area.set_center_y(125);

	PathManager pman(argv[0]);
	Sprite ch(pman.data_path("crosshairs.png","sprites"));
	Sprite s(pman.data_path("blue_full.png","sprites"));
	s.set_x(125);
	s.set_y(150);
	ch.set_priority(-1);
	window->register_graphic(&ch);
	window->register_graphic(&hbar);
	window->register_graphic(&vbar);
	window->register_graphic(&area);
	area.get_group()->add_graphic(&s);

	bool running = true;
	uint64_t lasttick = get_ticks();
	while(running) {
		uint64_t currenttick = get_ticks();
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
				case SDLK_DOWN:
					vbar.scroll(0.1);
					break;
				case SDLK_UP:
					vbar.scroll(-0.1);
					break;
				case SDLK_RIGHT:
					hbar.scroll(0.1);
					break;
				case SDLK_LEFT:
					hbar.scroll(-0.1);
					break;
				default: break;
				}
				break;
			case SDL_MOUSEMOTION:
				ch.set_x(e.motion.x);
				ch.set_y(e.motion.y);
				hbar.mouse_motion_event(e.motion);
				vbar.mouse_motion_event(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				hbar.mouse_button_event(e.button);
				vbar.mouse_button_event(e.button);
				break;
			}
		}

		hbar.autoscroll(currenttick - lasttick);
		vbar.autoscroll(currenttick - lasttick);
		window->redraw();

		lasttick = currenttick;
	}

	return 0;
}
