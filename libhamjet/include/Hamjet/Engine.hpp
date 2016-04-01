#ifndef HAMJET_ENGINE_HPP
#define HAMJET_ENGINE_HPP

#include "SDL.h"

namespace Hamjet {

	class Engine {
	private:
		Engine();

	public:
		static Engine* init();
	};

}

#endif