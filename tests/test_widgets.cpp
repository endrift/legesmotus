#include "gui/SDLWindow.hpp"
//#include "gui/GuiWindow.hpp"
#include "gui/ProgressBar.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ResourceCache.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow *window = SDLWindow::get_instance(300, 300, 24, SDLWindow::FLAG_VSYNC);
	GLESContext *ctx = window->get_context();
	ResourceCache *cache = new ResourceCache("data", ctx);
	//Font f("data/fonts/DustHomeMedium.ttf", 18, cache);
	Widget w0;
	w0.set_x(0);
	w0.set_y(150);
	/*GuiWindow gwin0(&w0);
	gwin0.set_title_text_font(&f);
	gwin0.set_title_text(L"Window 0");
	gwin0.set_width(200);
	gwin0.set_height(200);
	gwin0.set_x(-20);
	gwin0.set_y(-20);
	GuiWindow gwin1(&w0);
	gwin1.set_title_text_font(&f);
	gwin1.set_title_text(L"Window 1");
	gwin1.set_width(200);
	gwin1.set_height(200);
	gwin1.set_x(20);
	gwin1.set_y(20);*/
	ProgressBar p0(&w0);
	p0.set_width(150);
	p0.set_height(30);
	p0.set_color(Color(0.0f, 0.0f, 1.0f), Widget::COLOR_PRIMARY);
	p0.set_color(Color(0.5f, 0.5f, 1.0f), Widget::COLOR_SECONDARY);
	p0.set_x(150);
	ProgressBar p1(&w0);
	p1.set_width(150);
	p1.set_height(20);
	p1.set_orientation(true, false);
	p1.set_color(Color(1.0f, 0.0f, 0.0f), Widget::COLOR_PRIMARY);
	p1.set_color(Color(1.0f, 0.5f, 0.5f), Widget::COLOR_SECONDARY);
	p1.set_skew(1.0f);

	ctx->set_root_widget(&w0);

	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	int frame = 0;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}

		++frame;
		frame %= 100;

		p0.set_progress(frame / 100.0f);
		p1.set_progress(frame / 100.0f);

		window->redraw();

		SDL_Delay(20);
	}

	return 0;
}
