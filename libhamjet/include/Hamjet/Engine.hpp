#ifndef HAMJET_ENGINE_HPP
#define HAMJET_ENGINE_HPP

#include "SDL.h"

namespace Hamjet {
	class Application {
	public:
		virtual bool update(float dt) = 0;
		virtual void draw() = 0;
		virtual void onClick(int x, int y) = 0;
	};

	class Engine {
	public:
		SDL_Window *window;
		SDL_Renderer *windowRenderer;

	public:
		Engine();

		bool init(int windowWidth, int windowHeight);
		void term();
		void run(Application* app);
	};

}

#endif