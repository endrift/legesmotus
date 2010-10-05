#include "gui/SDLWindow.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ShaderSet.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/GraphicRegion.hpp"

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(512, 512, 24, 0);
	GLESContext* ctx = window->get_context();
	ResourceCache cache("data", ctx);
	Image tile("metal_hazard64.png", &cache, true);
	float c[] = { 4.0f, 4.0f, 0.5f, 1.0f };
	GraphicRegion tile_s(&tile);
	tile_s.set_width(512);
	tile_s.set_height(512);
	ShaderSet* program = ctx->create_shader_set();
	PixelShader shader = ctx->load_pixel_shader("data/shaders/gl/impact.fs");
	program->attach_shader(shader);

	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	int frame = 0;

	program->link();
	ctx->bind_shader_set(program);
	program->set_variable("arc", 1.0f);
	program->set_variable("zoom", 1.5f);
	program->set_variable_2("center", 1, c);
	program->set_variable("tex", 0);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		ctx->load_identity();
		if (frame < 200) {
			program->set_variable("time", frame / 200.0f);
		} else if (frame < 250) {
			program->set_variable("time", 1.0f);
		} else {
			program->set_variable("time", 0);
		}

		frame = (frame + 1) % 300;
		ctx->bind_image(tile.get_handle());
		ctx->draw_bound_image_tiled(512, 512, 0, 0, 64, 64);

		SDL_GL_SwapBuffers();
		SDL_Delay(6);
	}

	return 0;
}
