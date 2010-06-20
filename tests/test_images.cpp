#include "client/GameWindow.hpp"
#include "gui/GLESContext.hpp"
#include "SDL.h"
#include "SDL_image.h"

#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(256, 256, 24, false);
	GLESContext ctx(256, 256);
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	bool mousedown = false;
	int wx = 64;
	int hy = 64;
	SDL_Surface* img = IMG_Load("data/sprites/metal_bgtile64.png");
	cout << hex << showbase;
	cout << img->format->Rmask << endl;
	cout << img->format->Gmask << endl;
	cout << img->format->Bmask << endl;
	cout << img->format->Amask << endl;
	DrawContext::Image imgh = ctx.gen_image(&wx, &hy, DrawContext::RGBA, (unsigned char*)img->pixels);
	SDL_FreeSurface(img);
	ctx.bind_image(imgh);
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
				case SDLK_q:
					ctx.unbind_image();
					break;
				case SDLK_w:
					ctx.bind_image(imgh);
					break;
				default:
					break;
			}
			case SDL_MOUSEMOTION:
				if (mousedown) {
					wx = 2*abs(e.motion.x - 128);
					hy = 2*abs(e.motion.y - 192);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mousedown = e.button.state == SDL_PRESSED;
				wx = 2*abs(e.button.x - 128);
				hy = 2*abs(e.button.y - 192);
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		ctx.load_identity();
		ctx.translate(96.0f, 64.0f);
		ctx.draw_bound_image(64, 64);
		ctx.translate(32.0f, 128.0f);
		ctx.draw_bound_image_region(wx, hy, 0, 0, 64, 64);

		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}

	return 0;
}
