#ifndef HAMJET_APPLICATION_HPP
#define HAMJET_APPLICATION_HPP

#include <memory>

#include "Hamjet/Engine.hpp"

#define APP_MAIN(cls) int main(int argc, char** argv) {\
                        std::unique_ptr<cls> app = std::make_unique<cls>();\
                        return app->run();\
                      }

namespace Hamjet {
	class Application {
	public:
		const std::unique_ptr<Engine> engine;

	public:
		Application();

		int run();
		void mainLoop();

		virtual void init(EngineConfig& config) = 0;
		virtual void postInit() = 0;
		virtual bool update(float dt) = 0;
		virtual void draw() = 0;
		virtual void onClick(int x, int y) = 0;
		virtual void onKeyDown() = 0;
	};
}

#endif