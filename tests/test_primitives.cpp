#include "client/GameWindow.hpp"
#include "gui/primitives.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	GLfloat w[2];
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, w);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(min(1.75f, w[1]));
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	int rotate = 0;
	while(running) {
		glPushMatrix();
		glTranslatef(150.0f, 150.0f, 0);
		//glScalef(10.0f, 10.0f, 1);
		glRotatef(rotate*0.1f, 0, 0, 1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_roundrect_fill(256.0f, 192.0f, 16.0f, 16);
		draw_roundrect_line(240.0f, 176.0f, 8.0f, 16);

		glPopMatrix();
		SDL_GL_SwapBuffers();
		SDL_Delay(10);
		//++rotate;
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
	}

	return 0;
}
