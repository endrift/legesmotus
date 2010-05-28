#include "client/GameWindow.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	GLESContext ctx(300, 300);
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	bool mousedown = false;
	float wx = 200;
	float hy = 200;
	GLfloat w[2];
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, w);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(min(1.5f, w[1]));
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_MOUSEMOTION:
				if (mousedown) {
					wx = 2*abs(e.motion.x - 150);
					hy = 2*abs(e.motion.y - 150);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mousedown = e.button.state == SDL_PRESSED;
				wx = 2*abs(e.button.x - 150);
				hy = 2*abs(e.button.y - 150);
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		ctx.set_draw_color(Color(1.0f, 1.0f, 1.0f, 0.5f));
		ctx.load_identity();
		ctx.translate(150.0f, 150.0f);
		ctx.draw_roundrect(wx, hy, 16.0f, 16);

		SDL_GL_SwapBuffers();
		SDL_Delay(6);
	}

	return 0;
}
