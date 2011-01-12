#include "gui/SDLWindow.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/Sprite.hpp"
#include "gui/GraphicContainer.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow *window = SDLWindow::get_instance(800, 600, 24, SDLWindow::FLAG_VSYNC);
	GLESContext *ctx = window->get_context();

	ResourceCache cache("data", ctx);
	Image blue("blue_full.png", &cache, true);

	Sprite blue_s(&blue);
	blue_s.set_center_x(32);
	blue_s.set_center_y(48);
	blue_s.set_x(75);
	blue_s.set_y(50);

	GraphicContainer g(true);
	g.set_width(150);
	g.set_height(100);
	g.set_x(25);
	g.set_y(150);
	g.add_graphic("blue", &blue_s);
	g.build_texture(&cache);

	GraphicContainer gs;
	gs.set_width(200);
	gs.set_height(400);
	gs.set_x(300);
	gs.set_y(100);
	gs.add_graphic("first", g.get_texture());
	gs.build_texture(&cache);

	SDL_ShowCursor(SDL_TRUE);

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
				switch(e.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				default:
					break;
				}
			}
		}

		g.get_graphic("blue")->set_rotation(-frame / 300.0f * 360.0f);

		++frame;
		frame %= 300;

		ctx->set_root_widget(&gs);
		window->redraw();

		ctx->set_root_widget(&g);
		window->redraw();

		SDL_Delay(20);
	}

	return 0;
}
