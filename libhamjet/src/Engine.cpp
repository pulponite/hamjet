#include "Hamjet/Engine.hpp"

namespace Hamjet {

	Engine::Engine() {}

	Engine* Engine::init() {
		Engine* e = new Engine();
		return e;
	}

}


int hello() {
	return 1;
}