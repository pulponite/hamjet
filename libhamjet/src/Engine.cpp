#include "Hamjet/Engine.hpp"

#include <lauxlib.h>
#include <stdio.h>

namespace Hamjet {

	Engine::Engine() {}

	bool Engine::init(EngineConfig& config) {
		luaState = luaL_newstate();

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
			printf("Error initializing SDL: %s\n", SDL_GetError());
			return false;
		}


		window = SDL_CreateWindow(config.windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			config.windowWidth, config.windowHeight, SDL_WINDOW_SHOWN);
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
}