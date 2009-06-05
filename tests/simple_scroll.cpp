#include "common/timer.hpp"
#include "common/PathManager.hpp"
#include "client/GameWindow.hpp"
#include "client/ScrollArea.hpp"
#include "client/ScrollBar.hpp"
#include "client/Sprite.hpp"

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300,300,24,false);
	ScrollBar bar;
	bar.set_x(280);
	bar.set_y(150);
	bar.register_window(window);
	bar.set_height(280);
	bar.set_section_color(ScrollBar::BUTTONS, Color(0.5,0.5,0.5));
	bar.set_section_color(ScrollBar::TRACK, Color(0.2,0.2,0.2));
	bar.set_section_color(ScrollBar::TRACKER, Color(0.4,0.4,0.4));
	ScrollArea area(250,250,1000,&bar);
	area.set_x(150);
	area.set_y(150);
	area.set_center_x(125);
	area.set_center_y(125);

	PathManager pman(argv[0]);
	Sprite ch(pman.data_path("crosshairs.png","sprites"));
	Sprite s(pman.data_path("blue_full.png","sprites"));
	s.set_x(125);
	s.set_y(500);
	ch.set_priority(-1);
	window->register_graphic(&ch);
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
					bar.scroll(0.1);
					break;
				case SDLK_UP:
					bar.scroll(-0.1);
					break;
				default: break;
				}
				break;
			case SDL_MOUSEMOTION:
				ch.set_x(e.motion.x);
				ch.set_y(e.motion.y);
				bar.mouse_motion_event(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				bar.mouse_button_event(e.button);
				break;
			}
		}

		bar.autoscroll(currenttick - lasttick);
		window->redraw();

		lasttick = currenttick;
	}

	return 0;
}
