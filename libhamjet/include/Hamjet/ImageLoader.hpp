#ifndef HAMJET_IMAGELOADER_HPP
#define HAMJET_IMAGELOADER_HPP

#include <SDL.h>
#include <stdio.h>

namespace Hamjet {

	class ImageLoader {
	public:
		static SDL_Surface* loadPng(const char* filename);
		static SDL_Surface* loadPng(const char* filename, uint32_t pixelFormat);
		static SDL_Surface* loadPng(FILE* file);
		static SDL_Surface* loadPng(FILE* file, uint32_t pixelFormat);
	};

}

#endif

