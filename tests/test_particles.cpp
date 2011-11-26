#include "gui/SDLWindow.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ShaderSet.hpp"
#include "gui/ResourceCache.hpp"
#include "gui/Image.hpp"
#include "gui/GraphicRegion.hpp"
#include "gui/GraphicContainer.hpp"
#include "gui/SimpleRadialEmitter.hpp"
#include "gui/ParticleManager.hpp"
#include "common/math.hpp"
#include "gui/DrawContext.hpp"

using namespace LM;
using namespace std;

void add_emitter(ParticleManager* particle_manager, SimpleRadialEmitterSettings* settings, Image* particle) {
	SimpleRadialEmitter* simple_emitter = new SimpleRadialEmitter(particle_manager, Point(rand() % 400 + 50, rand() % 400 + 50), particle, DrawContext::BLEND_ADD);
	
	simple_emitter->init(settings);
	
	particle_manager->add_emitter(simple_emitter);
}

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(512, 512, 24, Window::FLAG_VSYNC);
	GLESContext* ctx = window->get_context();
	ResourceCache cache("data", ctx);
	Image tile("metal_hazard64.png", &cache, true);
	Image particle("blue_gate.png", &cache, true);
	
	Widget w0;
	w0.set_x(0);
	w0.set_y(0);
	
	GraphicRegion tile_s(&tile);
	float c[] = { 4.0f, 8.0f, 0.5f, 1.0f };
	float s[] = { 9.0f, 14.0f };
	ShaderSet* program = ctx->create_shader_set();
	PixelShader shader = ctx->load_pixel_shader("data/shaders/gl/impact");
	program->attach_shader(shader);

	tile_s.set_width(512);
	tile_s.set_height(512);
	tile_s.set_image_height(32);
	tile_s.set_shader_set(program);

	GraphicContainer g(true, &w0);
	g.add_graphic("tile", &tile_s, -1);
	
	SimpleRadialEmitterSettings* settings = new SimpleRadialEmitterSettings();
	settings->particle_speed = 100.0f;
	settings->speed_variance = 0.05f;
	settings->spawn_per_second = 1000.0f;
	settings->spawn_variance = 1.0f;
	settings->lifetime_millis = 500;
	settings->lifetime_variance = 100;
	settings->rotation_rads = 0.0f;
	settings->rotation_variance = 2 * M_PI;
	settings->global_force = Point(0.0f,0.0f);
	settings->max_spawn = 500;
	settings->emitter_stop_spawning_millis = -1;
	settings->emitter_lifetime_millis = -1;
	
	ParticleManager* particle_manager = new ParticleManager(&w0, 100, true);
	
	ctx->set_root_widget(&w0);

	SDL_ShowCursor(SDL_TRUE);

	bool running = true;
	int frame = 0;
	uint64_t last_frame_time = get_ticks();

	program->link();
	ctx->bind_shader_set(program);
	program->set_variable("arc", 0.3f);
	program->set_variable_2("size", 1, s);
	program->set_variable("speed", 2.0f);
	program->set_variable("offset", 0.0f);
	program->set_variable("waves", 2.0f);
	program->set_variable_2("center", 1, c);
	program->set_variable("tex", 0);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				default:
					break;
				}
			}
		}
		
		//ctx->bind_shader_set(program);
		//if (frame < 100) {
			//program->set_variable("time", frame / 80.0f);
		//} else if (frame < 150) {
		//	program->set_variable("time", 1.0f);
		//} else {
		//	program->set_variable("time", 0);
		//}

		frame = (frame + 1) % 80;
		
		if (frame % 10 == 0) {
			add_emitter(particle_manager, settings, &particle);
		}
		
		particle_manager->update(get_ticks() - last_frame_time);
		last_frame_time = get_ticks();
		
		window->redraw();
		
		SDL_Delay(6);
	}
	
	delete particle_manager;
	
	delete settings;

	return 0;
}
