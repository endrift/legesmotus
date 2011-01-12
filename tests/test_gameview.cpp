#include "gui/SDLWindow.hpp"
#include "gui/GameView.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/Sprite.hpp"
#include "gui/GraphicContainer.hpp"
#include "gui/GraphicRegion.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow *window = SDLWindow::get_instance(800, 600, 24, SDLWindow::FLAG_VSYNC);
	GLESContext *ctx = window->get_context();
	ResourceCache cache("data", ctx);
	GameView gv("gv", &cache, 600, 400, 100);
	Image blue("blue_full.png", &cache, true);
	Image tile("metal_bgtile64.png", &cache, true);

	Sprite blue_s(&blue);
	blue_s.set_center_x(24);
	blue_s.set_center_y(32);
	blue_s.set_x(64);
	blue_s.set_y(64);
	GraphicRegion tile_s(&tile);
	tile_s.set_width(800);
	tile_s.set_height(800);
	tile_s.set_x(64);
	tile_s.set_y(64);

	GraphicContainer bg(true);
	bg.add_graphic("tile", &tile_s, -1);
	gv.add_child(&bg);

	GraphicContainer g(true);
	g.add_graphic("blue", &blue_s, 1);
	g.set_width(128);
	g.set_height(128);
	g.set_x(32);
	g.set_y(32);
	g.build_texture(&cache);
	gv.add_child(&g);

	ctx->set_root_widget(&gv);

	SDL_ShowCursor(SDL_TRUE);

	int wx = 800;
	int hy = 600;
	bool running = true;
	bool mousedown = false;

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
			case SDL_MOUSEMOTION:
				if (mousedown) {
					wx = abs(e.motion.x);
					hy = abs(e.motion.y);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mousedown = e.button.state == SDL_PRESSED;
				wx = abs(e.button.x);
				hy = abs(e.button.y);
				break;
			}
		}

		window->redraw();

		SDL_Delay(20);
	}

	return 0;
}
