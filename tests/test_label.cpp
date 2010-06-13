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
	GameWindow *window = GameWindow::get_instance(500, 100, 24, GameWindow::VSYNC);
	GLESContext ctx(500, 100);
	
	Font font(string("data/fonts/JuraMedium.ttf"), 30, &ctx);
	Label l(string("This is text! Oh god..."), &font);
	l.set_align(Label::ALIGN_CENTER);
	
	SDL_ShowCursor(SDL_TRUE);

	glClearColor(0.5, 0.5, 0.5, 1.0);

	bool running = true;
	int phase = 0;
	ctx.translate(250, 60);
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

		float f = sin(phase*M_PI/50.0f);
		float c = (1-f)*0.5f;
		ctx.scale(1, 1/(c+0.8));
		l.set_color(Color(c, c, c, 1));
		l.set_tracking(4.0f*c);
		l.redraw(&ctx);
		ctx.scale(1, (c+0.8));

		SDL_GL_SwapBuffers();
		SDL_Delay(10);
		phase = (phase+1)%100;
	}

	return 0;
}
