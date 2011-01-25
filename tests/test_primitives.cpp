#include "common/Point.hpp"
#include "gui/SDLWindow.hpp"
#include "gui/GLESContext.hpp"
#include <iostream>
#include <cmath>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(300, 300, 24, 0);
	GLESContext* ctx = window->get_context();
	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	bool mousedown = false;
	float wx = 0;
	float hy = -10;
	GLfloat w[2];
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, w);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(min(1.5f, w[1]));
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	float points[6];
	Point ppoints[3];
	ppoints[0].x = 1;
	ppoints[1].x = -1;
	ppoints[2].x = 0;
	ppoints[0].y = sqrt(3)/3;
	ppoints[1].y = sqrt(3)/3;
	ppoints[2].y = -2*sqrt(3)/3;
	float r = 0;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_MOUSEMOTION:
				if (mousedown) {
					wx = 2*(e.motion.x - 150);
					hy = 2*(e.motion.y - 150);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mousedown = e.button.state == SDL_PRESSED;
				wx = 2*(e.button.x - 150);
				hy = 2*(e.button.y - 150);
				break;
			}
		}

		Point localpoints[3] = ppoints;

		r = atan2(wx, -hy);
		glClear(GL_COLOR_BUFFER_BIT);

		ctx->set_draw_color(Color(1.0f, 1.0f, 1.0f, 0.5f));
		ctx->set_secondary_color(Color(1.0f, 1.0f, 1.0f, 0.0f));
		ctx->load_identity();
		ctx->translate(150.0f, 150.0f);
		ctx->scale(100.0f, 100.0f);

		localpoints[0].rotate(r);
		localpoints[1].rotate(r);
		localpoints[2].rotate(r);

		points[0] = localpoints[0].x;
		points[1] = localpoints[0].y;
		points[2] = localpoints[1].x;
		points[3] = localpoints[1].y;
		points[4] = localpoints[2].x;
		points[5] = localpoints[2].y;
		ctx->draw_stroke(points, 3, 0.0, 0.1, true);

		ctx->scale(0.5, 0.5);
		localpoints[0].rotate(M_PI);
		localpoints[1].rotate(M_PI);
		localpoints[2].rotate(M_PI);

		points[0] = localpoints[0].x;
		points[1] = localpoints[0].y;
		points[2] = localpoints[1].x;
		points[3] = localpoints[1].y;
		points[4] = localpoints[2].x;
		points[5] = localpoints[2].y;
		ctx->draw_stroke(points, 3, 0.0, 0.1, true);

		ctx->use_secondary_color(true);
		ctx->draw_ring_fill(1.0f, 1.0f, 0.5f, 32);
		ctx->use_secondary_color(false);

		SDL_GL_SwapBuffers();
		SDL_Delay(6);
	}

	return 0;
}
