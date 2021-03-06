#include "gui/SDLWindow.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(300, 300, 24, 0);
	GLESContext* ctx = window->get_context();
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	float wx = 75;
	float hy = 75;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ctx->set_draw_color(Color(0.5f, 0.25f, 0.0f, 1.0f));
		ctx->load_identity();
		ctx->translate(150.0f, 0.0f);

		ctx->translate(0, 37.5f);
		ctx->set_blend_mode(DrawContext::BLEND_NORMAL);
		ctx->translate(-25.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);
		ctx->translate(50.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);

		ctx->translate(-25.0, 25.0f);
		ctx->set_blend_mode(DrawContext::BLEND_ADD);
		ctx->translate(-25.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);
		ctx->translate(50.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);

		ctx->translate(-25.0, 25.0f);
		ctx->set_blend_mode(DrawContext::BLEND_SCREEN);
		ctx->translate(-25.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);
		ctx->translate(50.0f, 25.0f);
		ctx->draw_rect_fill(wx, hy);

		SDL_GL_SwapBuffers();
		SDL_Delay(6);
	}

	return 0;
}
