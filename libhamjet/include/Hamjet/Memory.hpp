#ifndef HAMJET_MEMORY_HPP
#define HAMJET_MEMORY_HPP

#include <memory>
#include <SDL.h>

namespace Hamjet {

	typedef std::shared_ptr<SDL_Surface> SDL_Surface_Ptr;
	typedef std::shared_ptr<SDL_Texture> SDL_Texture_Ptr;

	class Memory {
	public:
		static SDL_Surface_Ptr wrap(SDL_Surface* surface);
		static SDL_Texture_Ptr wrap(SDL_Texture* texture);
	};

}

#endif