#include "gui/SDLWindow.hpp"
#include "gui/DrawContext.hpp"
#include "gui/SDLInputDriver.hpp"
#include "gui/InputSink.hpp"
#include "common/timer.hpp"
#include <iostream>

using namespace LM;
using namespace std;

class I : public InputSink {
public:
	bool running;

	virtual void key_pressed(const KeyEvent& ke) {
		wcout << L"Key pressed: " << get_key_name(ke) << " (Raw: " << ke.raw << ")" << endl;
		if (ke.type == KEY_ESCAPE) {
			running = false;
		}
	}

	virtual void mouse_moved(const MouseMotionEvent& mme) {
		cout << "Mouse moved " << mme.dx << ", " << mme.dy << " to " << mme.x << ", " << mme.y << endl;
	}

	virtual void mouse_clicked(const MouseButtonEvent& mbe) {
		cout << "Mouse button " << mbe.button << (mbe.down?" pressed":" released") << endl;
	}

	virtual void system_event(const SystemEvent& mbe) {
		running = false;
	}
};

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(100, 100, 32, 0);
	DrawContext* ctx = window->get_context();
	SDLInputDriver input;
	I i;
	i.running = true;
	input.set_sink(&i);

	while(i.running) {
		input.update();
		msleep(20);
	}

	return 0;
}
