#include "Hamjet/ImageLoader.hpp"

#include "png.h"

#define PNG_SIG_SIZE 8

namespace Hamjet {
	SDL_Surface* ImageLoader::loadPng(const char* filename) {
		return loadPng(filename, SDL_PIXELFORMAT_UNKNOWN);
	}

	SDL_Surface* ImageLoader::loadPng(const char* filename, uint32_t pixelFormat) {
		FILE* file = fopen(filename, "rb");
		if (file == NULL) {
			return NULL;
		}

		SDL_Surface* s = loadPng(file, pixelFormat);
		fclose(file);

		return s;
	}

	SDL_Surface* ImageLoader::loadPng(FILE* file) {
		return loadPng(file, SDL_PIXELFORMAT_UNKNOWN);
	}

	SDL_Surface* ImageLoader::loadPng(FILE* file, uint32_t pixelFormat) {
		// References:
		// * https://gist.github.com/niw/5963798
		// * http://www.libpng.org/pub/png/libpng-1.4.0-manual.pdf
		// * http://www.libpng.org/pub/png/libpng-manual.txt

		if (!file) {
			return NULL;
		}

		// Ensure the PNG signature is correct
		unsigned char header[PNG_SIG_SIZE];
		fread(&header, 1, PNG_SIG_SIZE, file);
		bool is_png = !png_sig_cmp(header, 0, PNG_SIG_SIZE);
		if (!is_png) {
			return NULL;
		}


		// Set up structs
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png == NULL) {
			return NULL;
		}

		png_infop info = png_create_info_struct(png);
		if (info == NULL)
		{
			png_destroy_read_struct(&png, NULL, NULL);
			return NULL;
		}

		png_infop end_info = png_create_info_struct(png);
		if (!end_info)
		{
			png_destroy_read_struct(&png, &info, NULL);
			return NULL;
		}

		if (setjmp(png_jmpbuf(png))) {
			png_destroy_read_struct(&png, &info, &end_info);
			return NULL;
		}

		png_init_io(png, file);
		png_set_sig_bytes(png, PNG_SIG_SIZE);

		png_read_info(png, info);

		int width = png_get_image_width(png, info);
		int height = png_get_image_height(png, info);
		png_byte bitDepth = png_get_bit_depth(png, info);
		png_byte colorType = png_get_color_type(png, info);

		// Apparently you can have 16bits per color channel in png files.
		// We don't want this.
		if (bitDepth == 16) {
			png_set_strip_16(png);
		}

		// Don't use palettes, read as absolute color data
		if (colorType == PNG_COLOR_TYPE_PALETTE) {
			png_set_palette_to_rgb(png);
		}

		// Increase the bit depth up to 8 from greyscale images
		if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) {
			png_set_expand_gray_1_2_4_to_8(png);
		}

		// Add alpha channel if absent.
		if (png_get_valid(png, info, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(png);
		}

		// Fill alpha channel with 0xFF if it wasn't present in the source image
		if (colorType == PNG_COLOR_TYPE_RGB ||
			colorType == PNG_COLOR_TYPE_GRAY ||
			colorType == PNG_COLOR_TYPE_PALETTE) {
			png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
		}

		// Read greyscale as RGB
		if (colorType == PNG_COLOR_TYPE_GRAY ||
			colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
			png_set_gray_to_rgb(png);
		}

		png_read_update_info(png, info);

		png_size_t rowbytes = png_get_rowbytes(png, info);
		png_byte* bytes = new png_byte[height * rowbytes];
		png_bytep* rows = new png_bytep[height];
		for (int i = 0; i < height; i++) {
			rows[i] = bytes + (i * rowbytes);
		}

		png_read_image(png, rows);

		SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		for (int i = 0; i < height; i++) {
			SDL_memcpy((char*)surface->pixels + (i * surface->pitch), rows[i], rowbytes);
		}

		delete[] rows;
		delete[] bytes;

		if (pixelFormat != SDL_PIXELFORMAT_UNKNOWN) {
			SDL_PixelFormat* format = SDL_AllocFormat(pixelFormat);
			SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, format, 0);
			SDL_FreeFormat(format);
			SDL_FreeSurface(surface);
			surface = convertedSurface;
		}

		return surface;
	}
}