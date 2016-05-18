#include "Hamjet/Application.hpp"

namespace Hamjet {
	Application::Application() : engine(std::make_unique<Engine>()) {}

	int Application::run() {
		EngineConfig config;
		init(config);
		
		if (!engine->init(config)) {
			return 1;
		}

		postInit();

		mainLoop();

		engine->term();
		return 0;
	}

	void Application::mainLoop() {
		bool cont = true;
		uint32_t lastFrameTime = SDL_GetTicks();

		while (cont) {
			uint32_t thisTime = SDL_GetTicks();
			uint32_t timeDelta = thisTime - lastFrameTime;
			lastFrameTime = thisTime;

			float dt = (fmax(timeDelta, 1) / 1000.0f);

			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					cont = false;
					break;
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN) {
					onClick(e.button.x, e.button.y);
				}
				else if (e.type == SDL_KEYDOWN) {
					onKeyDown();
				}
			}

			cont = cont && update(dt);
			draw();
		}
	}
}