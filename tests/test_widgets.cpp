#include "client/GameWindow.hpp"
#include "gui/Widget.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, GameWindow::VSYNC);
	GLESContext ctx(300, 300);
	Widget w0;
	Widget w1(&w0);
	Widget w2(&w0);
	Widget w3(&w1);
	Widget w4(&w1);
	w0.set_x(150);
	w0.set_y(150);
	w0.set_width(50);
	w0.set_height(50);
	w1.set_x(20);
	w1.set_y(20);
	w1.set_width(20);
	w1.set_height(20);
	w2.set_x(-20);
	w2.set_y(-20);
	w2.set_width(20);
	w2.set_height(20);
	w3.set_x(5);
	w3.set_y(5);
	w3.set_width(16);
	w3.set_height(16);
	w4.set_x(-5);
	w4.set_y(-5);
	w4.set_width(16);
	w4.set_height(16);
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_MOUSEMOTION:
				w0.mouse_moved(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				w0.mouse_clicked(e.button.x, e.button.y, e.button.state == SDL_PRESSED);
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		w0.draw(&ctx);

		SDL_GL_SwapBuffers();
		SDL_Delay(200);
	}

	return 0;
}
