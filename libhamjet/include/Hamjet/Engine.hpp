#ifndef HAMJET_ENGINE_HPP
#define HAMJET_ENGINE_HPP

#include <SDL.h>
#include <string>
#include <lua.h>
#include <memory>

namespace Hamjet {
	struct EngineConfig {
		unsigned int windowWidth = 640;
		unsigned int windowHeight = 480;
		std::string windowTitle = "Hamjet";
	};

	class Engine {
	public:
		SDL_Window *window;
		SDL_Renderer *windowRenderer;
		lua_State* luaState;

	public:
		Engine();

		bool init(EngineConfig& config);
		void term();
	};
}

#endif