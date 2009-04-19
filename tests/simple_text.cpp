#include "client/Font.hpp"
#include "client/GameWindow.hpp"
#include "client/TextManager.hpp"
#include "client/ClientSDL.hpp"
#include <string>

using namespace std;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(300, 60, 24, false);
	Font font("data/fonts/JuraMedium.ttf",12);
	TextManager texts(&font);
	texts.place_string("FIRST POST",150.0,0.0,TextManager::CENTER,window);
	texts.set_active_color(0x87/255.0,0x68/255.0,0x16/255.0);
	texts.place_string("That bag is brown",0.0,15.0,TextManager::LEFT,window);
	texts.set_active_color(0x6a/255.0,0xb2/255.0,0xe6/255.0);
	texts.place_string("The sky is blue",300.0,30.0,TextManager::RIGHT,window);
	texts.set_active_color(1.0,1.0,1.0);
	texts.set_active_color(0.8,0.8,0.8);
	texts.place_string("SUPERIMPOSED",150.0,44.0,TextManager::CENTER,window);
	texts.set_active_alpha(0.8);
	texts.set_active_color(0.0,0.0,0.0);
	texts.place_string("SUPERIMPOSED",151.0,45.0,TextManager::CENTER,window);
	bool running = true;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				} break;
			}
		}
		window->redraw();
		SDL_Delay(100);
	}
	texts.remove_all_strings(window);
	GameWindow::destroy_instance();
	return 0;
}
