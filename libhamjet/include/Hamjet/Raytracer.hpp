#ifndef HAMJET_RAYTRACER_HPP
#define HAMJET_RAYTRACER_HPP

#include "Hamjet/Math.hpp"

#include <stdint.h>

namespace Hamjet {
	class TraceObject {
	public:
		FSurface* surface;
		FVector3 color;

		TraceObject();
	};

	class TraceLight {
	public:
		FVector3 position;
		FVector3 color;

		TraceLight();
	};

	class Tracer {
	public:
		TraceObject* objects;
		TraceLight light;

		int objCount;

		Hamjet::FVector3 ambient;

	public:
		Tracer(int objs);
		~Tracer();

		void display(uint32_t* pixels, int width, int height, int pitch);
		TraceObject* cast(FRay& ray, TraceObject* exclude, float* d, FVector3* hitPoint, FVector3* normal);
		FRay rayForPixel(int x, int y, int width, int height);
		static uint32_t vecToARGB32(FVector3& vec);
	};
}

#endif