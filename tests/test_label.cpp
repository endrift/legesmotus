#include "client/GameWindow.hpp"
#include "gui/Font.hpp"
#include "gui/Label.hpp"
#include "gui/GLESContext.hpp"
#include "client/Sprite.hpp"
#include <iostream>
#include "SDL_image.h"
#include "common/math.hpp"

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, GameWindow::VSYNC);
	GLESContext ctx(300, 300);
	
	Font font(string("data/fonts/JuraMedium.ttf"), 18, &ctx);
	Label l(string("This is text! Oh god..."), &font);
	l.set_x(150);
	l.set_y(150);
	l.set_align(Label::ALIGN_CENTER);
	
	SDL_ShowCursor(SDL_TRUE);

	glClearColor(0.5, 0.5, 0.5, 1.0);

	bool running = true;
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

		glColor4d(1, 1, 1, 1);
		l.redraw(&ctx);

		SDL_GL_SwapBuffers();
		SDL_Delay(200);
	}

	return 0;
}
