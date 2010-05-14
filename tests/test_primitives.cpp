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
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
		glPushMatrix();
		glTranslatef(150.0f, 150.0f, 0);
		glScalef(50.0f, 50.0f, 1);
		glRotatef(rotate*0.1f, 0, 0, 1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		glTranslatef(1.0f, 1.0f, 0);
		draw_arc(0.25f, 1.0f, 1.0f, 16);

		glRotatef(-90, 0, 0, 1.0f);
		glTranslatef(2.0f, 0, 0);
		draw_arc(0.25f, 1.0f, 1.0f, 16);

		glRotatef(-90, 0, 0, 1.0f);
		glTranslatef(2.0f, 0, 0);
		draw_arc(0.25f, 1.0f, 1.0f, 16);

		glRotatef(-90, 0, 0, 1.0f);
		glTranslatef(2.0f, 0, 0);
		draw_arc(0.25f, 1.0f, 1.0f, 16);
		glPopMatrix();

		glTranslatef(1.5f, 0, 0);
		draw_rect_fill(1.0f, 2.0f);

		glTranslatef(-3.0f, 0, 0);
		draw_rect_fill(1.0f, 2.0f);

		glTranslatef(1.5f, 1.5f, 0);
		draw_rect_fill(2.0f, 1.0f);

		glTranslatef(0, -3.0f, 0);
		draw_rect_fill(2.0f, 1.0f);


		glTranslatef(0, 1.5f, 0);
		draw_rect_fill(2.0f, 2.0f);

		draw_line(1.0f, -2.0f, -1.0f, -2.0f);
		draw_line(1.0f, 2.0f, -1.0f, 2.0f);
		draw_line(2.0f, 1.0f, 2.0f, -1.0f);
		draw_line(-2.0f, 1.0f, -2.0f, -1.0f);

		glPopMatrix();
		SDL_GL_SwapBuffers();
		SDL_Delay(10);
		++rotate;
	}

	return 0;
}
