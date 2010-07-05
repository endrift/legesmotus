#include "client/GameWindow.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/Sprite.hpp"
#include "gui/GraphicRegion.hpp"
#include "gui/GLESContext.hpp"
#include <cmath>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	GLESContext ctx(300, 300);
	ResourceCache cache("data", &ctx);
	Image blue("blue_armless.png", &cache, true);
	Image tile("metal_bgtile.png", &cache, true);
	Sprite blue_s(&blue);
	GraphicRegion tile_s(&tile);
	blue_s.set_center_x(32);
	blue_s.set_center_y(48);
	tile_s.set_center_x(128);
	tile_s.set_center_y(128);
	tile_s.set_width(256);
	tile_s.set_height(256);
	tile_s.set_x(250);
	tile_s.set_y(250);
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
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = 0;
				} break;
			}
		}

		blue_s.set_rotation(-frame / 400.0 * 360);
		blue_s.set_x(cos(frame / 400.0 * 2*M_PI)*100+250);
		blue_s.set_y(sin(frame / 400.0 * 2*M_PI)*100+250);
		blue_s.set_scale_x((sin(frame / 200.0 * 2*M_PI)+1)/2.0);
		blue_s.set_scale_y((sin(frame / 200.0 * 2*M_PI)+1)/2.0);
		tile_s.set_image_width((sin(frame / 200.0 * 2*M_PI)+3)*16.0);
		tile_s.set_image_height((sin(frame / 200.0 * 2*M_PI)+3)*16.0);
		tile_s.set_image_x(128);
		tile_s.set_image_y(128);
		++frame;
		frame %= 400;

		glClear(GL_COLOR_BUFFER_BIT);

		tile_s.draw(&ctx);
		blue_s.draw(&ctx);

		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}
	return 0;
}
