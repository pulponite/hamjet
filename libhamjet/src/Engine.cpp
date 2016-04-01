#include "Hamjet/Engine.hpp"

#include <stdio.h>

namespace Hamjet {

	Engine::Engine() {}

	bool Engine::init(int windowWidth, int windowHeight) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
			printf("Error initializing SDL: %s\n", SDL_GetError());
			return false;
		}


		window = SDL_CreateWindow("Rays!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("Error creating window: %s\n", SDL_GetError());
			return false;
		}

		windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (windowRenderer == NULL) {
			return false;
		}
		return true;
	}

	void Engine::term() {
		if (windowRenderer != NULL) {
			SDL_DestroyRenderer(windowRenderer);
		}
		if (window != NULL) {
			SDL_DestroyWindow(window);
		}
		SDL_Quit();
	}

	void Engine::run(Application* a) {
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
					a->onClick(e.button.x, e.button.y);
				}
				else if (e.type == SDL_KEYDOWN) {
					a->onKeyDown();
				}
			}

			cont = cont && a->update(dt);
			a->draw();
		}
	}
}