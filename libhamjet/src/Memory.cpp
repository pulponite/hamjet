#include "Hamjet/Memory.hpp"

namespace Hamjet {
	SDL_Surface_Ptr Memory::wrap(SDL_Surface* surface) {
		return SDL_Surface_Ptr(surface, SDL_FreeSurface);
	}

	SDL_Texture_Ptr Memory::wrap(SDL_Texture* texture) {
		return SDL_Texture_Ptr(texture, SDL_DestroyTexture);
	}
}