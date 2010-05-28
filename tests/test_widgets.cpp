#include "client/GameWindow.hpp"
#include "gui/Widget.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	GLESContext ctx(300, 300);
	Widget w(NULL);
	w.set_x(150);
	w.set_y(150);
	w.set_width(50);
	w.set_height(50);
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_MOUSEMOTION:
				w.mouse_moved(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				w.mouse_clicked(e.button.x, e.button.y, e.button.state == SDL_PRESSED);
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		w.redraw(&ctx);

		SDL_GL_SwapBuffers();
		SDL_Delay(6);
	}

	return 0;
}
