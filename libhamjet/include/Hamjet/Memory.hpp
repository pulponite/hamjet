#ifndef HAMJET_MEMORY_HPP
#define HAMJET_MEMORY_HPP

#include <memory>
#include <SDL.h>

namespace Hamjet {

	typedef std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> SDL_Surface_Ptr;
	typedef std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> SDL_Texture_Ptr;

}

#endif