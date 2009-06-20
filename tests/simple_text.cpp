#include "client/Font.hpp"
#include "client/GameWindow.hpp"
#include "client/TextManager.hpp"
#include "client/ClientSDL.hpp"
#include "common/PathManager.hpp"
#include <string>

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(300, 75, 24, false);
	PathManager pman(argv[0]);
	Font font(pman.data_path("JuraMedium.ttf","fonts"),12);
	TextManager texts(&font,window);
	texts.place_string("FIRST POST",150.0,0.0,TextManager::CENTER);
	texts.set_active_color(0x87/255.0,0x68/255.0,0x16/255.0);
	texts.place_string("That bag is brown",0.0,15.0,TextManager::LEFT);
	texts.set_active_color(0x6a/255.0,0xb2/255.0,0xe6/255.0);
	texts.place_string("The sky is blue",300.0,30.0,TextManager::RIGHT);
	texts.set_active_color(1.0,1.0,1.0);
	texts.set_active_color(0.8,0.8,0.8);
	texts.place_string("SUPERIMPOSED",150.0,44.0,TextManager::CENTER);
	texts.set_active_alpha(0.8);
	texts.set_active_color(0.0,0.0,0.0);
	texts.place_string("SUPERIMPOSED",151.0,45.0,TextManager::CENTER);
	texts.set_shadow(true);
	texts.set_active_alpha(1.0);
	texts.set_active_color(1.0,0.0,0.0);
	texts.set_shadow_color(0.5,1.0,0.5);
	texts.place_string("Shadow",150.0,60.0,TextManager::CENTER);
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
	texts.remove_all_strings();
	GameWindow::destroy_instance();
	return 0;
}
