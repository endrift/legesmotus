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
	GLESContext *offscreen = new GLESContext(400, 300, true);

	ResourceCache cache("data", ctx);
	Image blue("blue_full.png", &cache, true);
	Image offimg(offscreen->get_image("screen", &cache));

	Sprite blue_s(&blue);
	blue_s.set_center_x(32);
	blue_s.set_center_y(48);
	blue_s.set_x(200);
	blue_s.set_y(150);
	blue_s.set_scale_x(0.5);
	blue_s.set_scale_y(0.5);

	GraphicContainer g;
	g.add_graphic("blue", &blue_s);

	offscreen->set_root_widget(&g);

	Sprite offimg_s(&offimg);
	offimg_s.set_scale_x(2);
	offimg_s.set_scale_y(2);

	GraphicContainer gs;
	gs.add_graphic("screen", &offimg_s);

	ctx->set_root_widget(&gs);

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

		g.get_graphic("blue")->set_rotation(-frame / 300.0 * 360);

		++frame;
		frame %= 300;

		offscreen->make_active();
		offscreen->redraw();

		ctx->make_active();
		window->redraw();

		SDL_Delay(20);
	}

	return 0;
}
