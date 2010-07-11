#include "client/GameWindow.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/Sprite.hpp"
#include "gui/GraphicContainer.hpp"
#include "gui/GraphicRegion.hpp"
#include "gui/GLESContext.hpp"
#include <cmath>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	GameWindow *window = GameWindow::get_instance(500, 500, 24, false);
	GLESContext ctx(500, 500);
	ResourceCache cache("data", &ctx);
	Image blue("blue_armless.png", &cache, true);
	Image bluef("blue_frontarm.png", &cache, true);
	Image blueb("blue_backarm.png", &cache, true);
	Image tile("metal_bgtile64.png", &cache, true);
	tile.add_mipmap("metal_bgtile.png", 1);
	Sprite blue_s(&blue);
	Sprite bluef_s(&bluef);
	Sprite blueb_s(&blueb);
	GraphicRegion tile_s(&tile);
	GraphicContainer g;
	Bone root;
	blue_s.get_bone()->set_parent(&root);
	blue_s.set_center_x(32);
	blue_s.set_center_y(48);
	bluef_s.get_bone()->set_parent(&root);
	bluef_s.set_center_x(46);
	bluef_s.set_center_y(30);
	bluef_s.set_x(13);
	bluef_s.set_y(-18);
	blueb_s.get_bone()->set_parent(&root);
	blueb_s.set_center_x(27);
	blueb_s.set_center_y(29);
	blueb_s.set_x(-2);
	blueb_s.set_y(-18);
	g.add_graphic("blue", &blue_s);
	g.add_graphic("bluef", &bluef_s, 1);
	g.add_graphic("blueb", &blueb_s, -1);
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

		g.set_x(cos(frame / 400.0 * 2*M_PI)*100+250);
		g.set_y(sin(frame / 400.0 * 2*M_PI)*100+250);
		root.set_rotation(-frame / 400.0 * 360);
		root.set_scale_x((cos(frame / 200.0 * 2*M_PI)+1)/2.0);
		root.set_scale_y((cos(frame / 200.0 * 2*M_PI)+1)/2.0);
		g.get_graphic("bluef")->set_rotation(-frame / 400.0 * 360);
		g.get_graphic("blueb")->set_rotation(-frame / 400.0 * 360);
		tile_s.set_image_width((cos(frame / 200.0 * 2*M_PI)+3)*16.0);
		tile_s.set_image_height((cos(frame / 200.0 * 2*M_PI)+3)*16.0);
		tile_s.set_image_x(128);
		tile_s.set_image_y(128);
		++frame;
		frame %= 400;

		glClear(GL_COLOR_BUFFER_BIT);

		ctx.load_identity();

		tile_s.draw(&ctx);
		g.draw(&ctx);

		ctx.load_identity();
		ctx.translate(g.get_x(), g.get_y());
		root.diagnostic_draw(&ctx);
		g.get_graphic("blue")->get_bone()->diagnostic_draw(&ctx);
		g.get_graphic("bluef")->get_bone()->diagnostic_draw(&ctx);
		g.get_graphic("blueb")->get_bone()->diagnostic_draw(&ctx);

		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}
	return 0;
}
