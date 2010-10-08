#include "gui/SDLWindow.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ShaderSet.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/GraphicRegion.hpp"
#include "gui/GraphicContainer.hpp"

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(512, 512, 24, Window::FLAG_VSYNC);
	GLESContext* ctx = window->get_context();
	ResourceCache cache("data", ctx);
	Image tile("metal_hazard64.png", &cache, true);
	GraphicRegion tile_s(&tile);
	float c[] = { 4.0f, 8.0f, 0.5f, 1.0f };
	float s[] = { 9.0f, 14.0f };
	ShaderSet* program = ctx->create_shader_set();
	PixelShader shader = ctx->load_pixel_shader("data/shaders/gl/impact.fs");
	program->attach_shader(shader);

	tile_s.set_width(512);
	tile_s.set_height(512);
	tile_s.set_image_height(32);
	tile_s.set_shader_set(program);

	GraphicContainer g;
	g.add_graphic("tile", &tile_s, -1);
	ctx->set_root_widget(&g);

	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	int frame = 0;

	program->link();
	ctx->bind_shader_set(program);
	program->set_variable("arc", 0.3f);
	program->set_variable_2("size", 1, s);
	program->set_variable("speed", 2.0f);
	program->set_variable("offset", 0.0f);
	program->set_variable("waves", 2.0f);
	program->set_variable_2("center", 1, c);
	program->set_variable("tex", 0);
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
		ctx->bind_shader_set(program);
		//if (frame < 100) {
			program->set_variable("time", frame / 80.0f);
		//} else if (frame < 150) {
		//	program->set_variable("time", 1.0f);
		//} else {
		//	program->set_variable("time", 0);
		//}

		frame = (frame + 1) % 80;
		
		window->redraw();
		SDL_Delay(6);
	}

	return 0;
}
