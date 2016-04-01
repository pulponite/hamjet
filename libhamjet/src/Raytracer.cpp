#include "Hamjet/Raytracer.hpp"

namespace Hamjet {
	TraceObject::TraceObject() : color(FVector3(0, 0, 0)) {}

	TraceLight::TraceLight() : color(FVector3(0, 0, 0)), position(FVector3(0, 0, 0)) {}

	Tracer::Tracer(int objs) : ambient(0.1, 0.2, 0.1) {
		objCount = objs;
		objects = new TraceObject[objs];

		light.position = FVector3(0, 5, 3);
	}

	Tracer::~Tracer() {
		delete[] objects;
	}

	void Tracer::display(uint32_t* pixels, int width, int height, int pitch) {
		FSphere sphere = FSphere(FVector3(0, 0, -3), 1);
		FPlane plane = FPlane(FPlane(FVector3::axisy(), -3.0f));

		float aspect = (float)width / (float)height;

		FVector3 camera = FVector3::zero();
		for (int y = 0; y < height; y++) {
			float ydir = 1.0f - ((2.0f * y) / (float)height);
			for (int x = 0; x < width; x++) {
				if (y == 141 && x == 160) {
					pitch = pitch;
				}

				float xdir = ((2.0f * aspect * x) / (float)width) - aspect;

				FVector3 rayDir = FVector3(xdir, ydir, -1);
				FRay ray = FRay(camera, rayDir);
				uint32_t color = 0xFF000000;

				float distance = 100;
				FVector3 normal(0, 0, 0);
				FVector3 hitPoint(0, 0, 0);
				TraceObject* obj = cast(ray, NULL, &distance, &hitPoint, &normal);

				if (obj != NULL) {

					FVector3 h_to_light = light.position.sub(hitPoint);
					FVector3 h_to_light_norm = h_to_light.normalize();

					FRay lightRay = Hamjet::FRay(hitPoint, h_to_light_norm);

					float lightDistance = 100;
					FVector3 lightNormal(0, 0, 0);
					FVector3 lightHitPoint(0, 0, 0);
					TraceObject* lightObj = cast(lightRay, obj, &lightDistance, &lightHitPoint, &lightNormal);

					if (lightObj == NULL || lightDistance > h_to_light.magnitude()) {
						float lightStrength = fmax(h_to_light_norm.dot(normal), 0);
						color = vecToARGB32(obj->color.mul(lightStrength).add(obj->color.mul(ambient)));
					}
				}


				uint32_t* pixel = pixels + (y * (pitch / 4)) + x;
				*pixel = color;
			}
		}
	}

	TraceObject* Tracer::cast(FRay& ray, TraceObject* exclude, float* d, FVector3* hitPoint, FVector3* normal) {
		float distance = 100;
		TraceObject* obj = NULL;
		FVector3 norm(0, 0, 0);

		for (int i = 0; i < objCount; i++) {
			float newDistance;
			FVector3 newNorm(0, 0, 0);
			if (&objects[i] != exclude && objects[i].surface->intersect(ray, &newDistance, &newNorm)) {
				if (newDistance < distance) {
					distance = newDistance;
					norm = newNorm;
					obj = &objects[i];
				}
			}
		}

		if (obj != NULL) {
			*d = distance;
			*hitPoint = ray.origin.add(ray.direction.mul(distance));
			*normal = norm;
		}

		return obj;
	}

	uint32_t Tracer::vecToARGB32(FVector3& vec) {
		return 0xFF000000 |
			((int)(fmax(fmin(vec.x, 1), 0) * 255) % 256) << 16 |
			((int)(fmax(fmin(vec.y, 1), 0) * 255) % 256) << 8 |
			((int)(fmax(fmin(vec.z, 1), 0) * 255) % 256);
	}

	Hamjet::FRay Tracer::rayForPixel(int x, int y, int width, int height) {
		float aspect = (float)width / (float)height;
		float ydir = 1.0f - ((2.0f * y) / (float)height);
		float xdir = ((2.0f * aspect * x) / (float)width) - aspect;

		return Hamjet::FRay(FVector3::zero(), FVector3(xdir, ydir, -1));
	}
}