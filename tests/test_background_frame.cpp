#include "gui/SDLWindow.hpp"
#include "gui/BackgroundFrame.hpp"
#include "gui/Button.hpp"
#include "gui/ProgressBar.hpp"
#include "gui/BackgroundFrame.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Font.hpp"
#include "gui/InputSink.hpp"
#include "gui/SDLInputDriver.hpp"
#include "gui/TextInput.hpp"
#include "gui/ScrollBar.hpp"
#include <iostream>

using namespace LM;
using namespace std;

class I : public InputSink, public TextInputCallback, public ButtonCallback, public ScrollBarCallback {
	public:
		bool running;
		Widget* m_root_widget;
		Widget* m_focused_item;
		TextInput* m_input;
		float mouse_x;
		float mouse_y;
	
		virtual ~I() {
		}

		virtual void key_pressed(const KeyEvent& ke) {
			if (m_focused_item != NULL) {
				m_focused_item->keypress(ke);
			}
			//wcout << L"Key pressed: " << get_key_name(ke) << " (Raw: " << ke.raw << ")" << endl;
		}

		virtual void mouse_moved(const MouseMotionEvent& mme) {
			//cout << "Mouse moved " << mme.dx << ", " << mme.dy << " to " << mme.x << ", " << mme.y << endl;
			m_root_widget->mouse_moved(mme.x, mme.y, mme.dx, mme.dy);
			mouse_x = mme.x;
			mouse_y = mme.y;
		}

		virtual void mouse_clicked(const MouseButtonEvent& mbe) {
			//cout << "Mouse button " << mbe.button << (mbe.down?" pressed":" released") << endl;
			m_root_widget->mouse_clicked(mouse_x, mouse_y, mbe.down, mbe.button);
		}

		virtual void system_event(const SystemEvent& mbe) {
			running = false;
		}
	
		virtual void set_root_widget(Widget* w) {
			m_root_widget = w;
		}
	
		virtual void set_focused_item(Widget* w) {
			m_focused_item = w;
		}
		
		virtual void input_box_clicked(int box_id, TextInput* input_box, std::wstring caption) {
			set_focused_item(input_box);
		}
		
		virtual void scroll_fraction_changed(int bar_id, ScrollBar* scroll_bar, float fraction) {
			if (!scroll_bar->is_horizontal()) {
				m_input->set_y(-125.0 + fraction * scroll_bar->get_virtual_size());
			}
		}
		
		virtual void input_completed(int box_id, TextInput* input_box, std::wstring caption) {
			set_focused_item(NULL);
			
			wcerr << "Input received: " << caption << endl;
			
			m_input->set_label(L"");
		}
		
		virtual void input_changed(int box_id, TextInput* input_box, std::wstring caption) {
		}
		
		virtual void button_pressed(int button_id, std::string caption) {
			//cerr << "Button pressed!" << " ID: " << button_id << " Caption: " << caption << endl;
		}
		
		virtual void button_released(int button_id, std::string caption) {
			if (m_focused_item != NULL) {
				m_focused_item = NULL;
			}
			
			m_input->set_label(L"");
		}
};

int main(int argc, char *argv[]) {
	SDLWindow *window = SDLWindow::get_instance(600, 600, 24, SDLWindow::FLAG_VSYNC);
	GLESContext *ctx = window->get_context();
	ResourceCache *cache = new ResourceCache("data", ctx);
	Widget w0;
	w0.set_x(0);
	w0.set_y(0);
	
	Font font("DustHomeMedium.ttf", 12, cache, false, NULL);
	
	SDLInputDriver input;
	I i;
	i.set_root_widget(&w0);
	i.set_focused_item(NULL);
	i.running = true;
	input.set_sink(&i);
	
	BackgroundFrame b1(&w0);
	b1.set_width(350);
	b1.set_height(350);
	b1.set_color(Color(1.0f, 0.0f, 0.0f, 0.6f), Widget::COLOR_PRIMARY);
	b1.set_color(Color(1.0f, 0.5f, 0.5f, 0.6f), Widget::COLOR_SECONDARY);
	b1.set_border_radius(10.0f);
	b1.set_border_max_roundness(16);
	b1.set_border_padding(5.0f);
	b1.set_thin_border(false);
	b1.set_x(200.0f);
	b1.set_y(200.0f);
	
	BackgroundFrame b2(&b1);
	b2.set_width(200);
	b2.set_height(200);
	b2.set_color(Color(1.0f, 0.0f, 0.0f, 0.6f), Widget::COLOR_PRIMARY);
	b2.set_color(Color(1.0f, 0.5f, 0.5f, 0.6f), Widget::COLOR_SECONDARY);
	b2.set_border_radius(10.0f);
	b2.set_border_max_roundness(16);
	b2.set_border_padding(5.0f);
	b2.set_thin_border(false);
	b2.set_x(0.0f);
	b2.set_y(36.0f);
	
	BackgroundFrame b3(&b2);
	b3.set_width(100);
	b3.set_height(100);
	b3.set_color(Color(1.0f, 0.0f, 0.0f, 0.6f), Widget::COLOR_PRIMARY);
	b3.set_color(Color(1.0f, 0.5f, 0.5f, 0.6f), Widget::COLOR_SECONDARY);
	b3.set_border_radius(10.0f);
	b3.set_border_max_roundness(16);
	b3.set_border_padding(5.0f);
	b3.set_thin_border(false);
	b3.set_x(0.0f);
	b3.set_y(0.0f);
	
	ScrollBar sb1(&b1);
	sb1.set_width(20);
	sb1.set_height(300);
	sb1.set_x(150);
	sb1.set_y(0);
	sb1.set_color(Color(0.0f, 0.0f, 0.7f, 1.0f), Widget::COLOR_PRIMARY);
	sb1.set_color(Color(0.3f, 0.3f, 0.7f, 1.0f), Widget::COLOR_SECONDARY);
	sb1.set_horizontal(false);
	sb1.set_border_radius(10.0f);
	sb1.set_border_padding(2.0f);
	sb1.set_virtual_size(900);
	sb1.set_callback_object(&i);
	sb1.set_pressed_color(Color(0.0f, 0.0f, 0.5f, 1.0f), Widget::COLOR_PRIMARY);
	sb1.set_pressed_color(Color(0.5f, 0.5f, 0.9f, 1.0f), Widget::COLOR_SECONDARY);
	sb1.set_hover_color(Color(0.0f, 0.0f, 0.9f, 1.0f), Widget::COLOR_PRIMARY);
	sb1.set_hover_color(Color(0.4f, 0.4f, 0.7f, 1.0f), Widget::COLOR_SECONDARY);
	sb1.set_inactive_color(Color(0.0f, 0.0f, 0.3f, 1.0f), Widget::COLOR_PRIMARY);
	sb1.set_inactive_color(Color(0.3f, 0.3f, 0.3f, 1.0f), Widget::COLOR_SECONDARY);
	sb1.set_change_on_pressed(true);
	sb1.set_change_on_hover(true);
	
	ScrollBar sb2(&b1);
	sb2.set_width(270);
	sb2.set_height(20);
	sb2.set_x(0);
	sb2.set_y(150);
	sb2.set_color(Color(0.0f, 0.0f, 0.7f, 1.0f), Widget::COLOR_PRIMARY);
	sb2.set_color(Color(0.3f, 0.3f, 0.7f, 1.0f), Widget::COLOR_SECONDARY);
	sb2.set_horizontal(true);
	sb2.set_border_radius(10.0f);
	sb2.set_border_padding(2.0f);
	sb2.set_virtual_size(700);
	sb2.set_callback_object(&i);
	sb2.set_pressed_color(Color(0.0f, 0.0f, 0.5f, 1.0f), Widget::COLOR_PRIMARY);
	sb2.set_pressed_color(Color(0.5f, 0.5f, 0.9f, 1.0f), Widget::COLOR_SECONDARY);
	sb2.set_inactive_color(Color(0.0f, 0.0f, 0.3f, 1.0f), Widget::COLOR_PRIMARY);
	sb2.set_inactive_color(Color(0.3f, 0.3f, 0.3f, 1.0f), Widget::COLOR_SECONDARY);
	sb2.set_change_on_pressed(true);
	sb2.set_state(ScrollBar::STATE_INACTIVE);
	
	TextInput ti1(&b1);
	ti1.set_width(250);
	ti1.set_height(30);
	ti1.set_color(Color(1.0f, 0.0f, 0.0f, 0.6f), Widget::COLOR_PRIMARY);
	ti1.set_color(Color(1.0f, 0.5f, 0.5f, 0.6f), Widget::COLOR_SECONDARY);
	ti1.set_border_radius(3.0f);
	ti1.set_border_max_roundness(16);
	ti1.set_border_padding(2.0f);
	ti1.set_thin_border(false);
	ti1.set_font(&font);
	ti1.set_x(0.0f);
	ti1.set_callback(&i);
	ti1.set_y(-125.0f);
	i.m_input = &ti1;
	
	Button bt1(&b1);
	bt1.set_width(80);
	bt1.set_height(30);
	bt1.set_normal_color(Color(0.0f, 0.0f, 0.7f, 1.0f), Widget::COLOR_PRIMARY);
	bt1.set_normal_color(Color(0.3f, 0.3f, 0.7f, 1.0f), Widget::COLOR_SECONDARY);
	bt1.set_pressed_color(Color(0.0f, 0.0f, 0.5f, 1.0f), Widget::COLOR_PRIMARY);
	bt1.set_pressed_color(Color(0.5f, 0.5f, 0.9f, 1.0f), Widget::COLOR_SECONDARY);
	bt1.set_hover_color(Color(0.0f, 0.0f, 0.9f, 1.0f), Widget::COLOR_PRIMARY);
	bt1.set_hover_color(Color(0.5f, 0.5f, 0.9f, 1.0f), Widget::COLOR_SECONDARY);
	bt1.set_inactive_color(Color(0.0f, 0.0f, 0.3f, 1.0f), Widget::COLOR_PRIMARY);
	bt1.set_inactive_color(Color(0.3f, 0.3f, 0.3f, 1.0f), Widget::COLOR_SECONDARY);
	bt1.set_change_on_hover(true);
	bt1.set_change_on_pressed(true);
	bt1.set_border_radius(3.0f);
	bt1.set_border_max_roundness(16);
	bt1.set_border_padding(2.0f);
	bt1.set_thin_border(false);
	bt1.set_x(-105.0f);
	bt1.set_y(-80.0f);
	bt1.set_label("Submit!", &font);
	bt1.set_callback(&i);
	
	ProgressBar p1(&b1);
	p1.set_width(150);
	p1.set_height(20);
	p1.set_orientation(true, false);
	p1.set_color(Color(1.0f, 0.0f, 0.0f), Widget::COLOR_PRIMARY);
	p1.set_color(Color(1.0f, 0.5f, 0.5f), Widget::COLOR_SECONDARY);
	p1.set_y(0.0f);
	p1.set_x(0.0f);
	p1.set_skew(1.0f);
	
	ProgressBar p0(&b3);
	p0.set_width(150);
	p0.set_height(30);
	p0.set_color(Color(0.0f, 0.0f, 1.0f), Widget::COLOR_PRIMARY);
	p0.set_color(Color(0.5f, 0.5f, 1.0f), Widget::COLOR_SECONDARY);
	p0.set_y(0.0f);
	p0.set_x(0.0f);

	ctx->set_root_widget(&w0);

	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	int frame = 0;
	
	uint64_t prev_frame = get_ticks();
	while(running) {
		input.update();
		/*SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_MOUSEBUTTONDOWN:
				w0.mouse_clicked(e.button.x, e.button.y, true, e.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				w0.mouse_clicked(e.button.x, e.button.y, false, e.button.button);
				break;
			case SDL_MOUSEMOTION:
				w0.mouse_moved(e.motion.x, e.motion.y, 0, 0);
				break;
			case SDL_KEYDOWN:
				m_focused_item.key_pressed(e.key.keysym.sym, e.key.keysym.unicode, true);
			case SDL_QUIT:
				running = 0;
				break;
			}
		}*/

		++frame;
		frame %= 100;

		p0.set_progress(frame / 100.0f);
		p1.set_progress(frame / 100.0f);
		
		//sb1.set_scroll_fraction(frame / 100.0f);
		//sb2.set_scroll_fraction(frame / 100.0f);
		
		w0.update(get_ticks() - prev_frame);

		prev_frame = get_ticks();

		window->redraw();
		
		running = i.running;

		SDL_Delay(20);
	}

	return 0;
}
