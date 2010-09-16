#include "gui/SDLWindow.hpp"
#include "gui/DrawContext.hpp"
#include "gui/SDLInputDriver.hpp"
#include <iostream>

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(100, 100, 32, 0);
	DrawContext* ctx = window->get_context();
	SDLInputDriver input;
	bool running = true;

	while(running) {
		if(input.update()) {
			KeyEvent ke;
			MouseMotionEvent mme;
			MouseButtonEvent mbe;

			while (input.poll_keys(&ke)) {
				wcout << L"Key pressed: " << get_key_name(ke) << endl;
				if (ke.type == KEY_ESCAPE) {
					running = false;
				}
			}

			while (input.poll_mouse_motion(&mme)) {
				cout << "Mouse moved " << mme.dx << ", " << mme.dy << " to " << mme.x << ", " << mme.y << endl;
			}
		}
	}

	return 0;
}
