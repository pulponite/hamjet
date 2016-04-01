#ifndef HAMJET_MATH_HPP
#define HAMJET_MATH_HPP

#include <cmath>

namespace Hamjet {

	template <typename T> class Vector4 {
	public:
		T x, y, z, w;

	public:
		Vector4(T mx, T my, T mz, T mw) {
			x = mx;
			y = my;
			z = mz;
			w = mw;
		}
	};

	template <typename T> class Vector3 {
	public:
		T x, y, z;

	public:
		Vector3(T x, T y, T z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3<T> add(Vector3<T>& other) const {
			return Vector3(x + other.x, y + other.y, z + other.z);
		}

		Vector3<T> sub(Vector3<T>& other) const {
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3<T> neg() const {
			return Vector3(-x, -y, -z);
		}

		Vector3<T> mul(T s) const {
			return Vector3(x * s, y * s, z * s);
		}

		Vector3<T> mul(Vector3<T>& other) const {
			return Vector3(x * other.x, y * other.y, z * other.z);
		}

		Vector3<T> div(T s) const {
			return Vector3(x / s, y / s, z / s);
		}

		T magnitude() const {
			return sqrt((x * x) + (y * y) + (z * z));
		}

		Vector3<T> normalize() const {
			T m = magnitude();
			if (m > 0) {
				return div(m);
			}
			else {
				return Vector3<T>(0, 0, 0);
			}
		}

		inline T dot(Vector3<T>& other) const {
			return (x * other.x) + (y * other.y) + (z * other.z);
		}

		T angle(Vector3<T>& other) const {
			T cosAngle = dot(other) / (magnitude() * other.magnitude);
			return acos(cosAngle);
		}

		Vector3<T> cross(Vector3<T>& other) const {
			return Vector3(
				(y * other.z) - (z * other.y),
				(z * other.x) - (x * other.z),
				(x * other.y) - (y * other.x)
			);
		}

		Vector3<T> project(Vector3<T>& other) const {
			T mag = other.magnitude;

			return other.normalize().mul((dot(other) / (mag * mag)));
		}

		Vector3<T> operator+(Vector3<T>& other) const {
			return add(other);
		}

		operator Vector4<T>() const {
			return Vector4<T>(x, y, z, 1);
		}

		static Vector3<T> zero() {
			return Vector3<T>(0, 0, 0);
		}

		static Vector3<T> axisy() {
			return Vector3<T>(0, 1, 0);
		}
	};
	typedef Vector3<float> FVector3;

	template <typename T> class Matrix44 {
	public:
		T m[4][4];

	public:
		Matrix44(T (&n)[4][4]) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					m[i][j] = n[i][j];
				}
			}
		}

		Matrix44<T> transpose() const {
			T n[4][4];

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					n[j][i] = m[j][i];
				}
			}

			return Matrix44(n);
		}

		Matrix44<T> add(Matrix44<T>& o) const {
			T n[4][4];

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					n[i][j] = m[i][j] + o.m[i][j];
				}
			}

			return Matrix44(n);
		}

		Matrix44<T> mul(Matrix44<T> &o) const {
			T n[4][4];

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					n[i][j] = 0;
					for (int k = 0; k < 4; k++) {
						n[i][j] += m[i][k] * o.m[k][j];
					}
				}
			}

			return Matrix44(n);
		}

		Vector4<T> mul(Vector4<T> &v) const {
			T o[4] = { v.x, v.y, v.z, v.w };
			T n[4];

			for (int i = 0; i < 4; i++) {
				n[i] = 0;
				for (int k = 0; k < 4; k++) {
					n[i] += m[i][k] * o[k];
				}
			}

			return Vector4<T>(n[0], n[1], n[2], n[3]);
		}


	};
	typedef Matrix44<float> FMatrix44;

	template <typename T> class Ray {
	public:
		Vector3<T> origin;
		Vector3<T> direction;

	public:
		Ray(Vector3<T> o, Vector3<T> d) : origin(o), direction(d) {}
	};
	typedef Ray<float> FRay;


	template <typename T> class Surface {
	public:
		virtual bool intersect(Ray<T>& ray, T* distance, Vector3<T>* normal) = 0;
	};
	typedef Surface<float> FSurface;

	template <typename T> class Plane : public Surface<T> {
	public:
		Vector3<T> normal;
		T offset;

	public:

		Plane(Vector3<T>& n, T o) : normal(n), offset(o) { }

		T distanceToPlane(Vector3<T>& p) const {
			return (normal.dot(p) - offset)
		}


		bool intersect(Ray<T>& ray, T* distance, Vector3<T>* norm) {
			Vector3<T> v = ray.direction.normalize();

			T n_dot_v = normal.dot(v);

			if (n_dot_v < 0) {
				T n_dot_s = normal.dot(ray.origin);
				T n_dot_q = normal.dot(normal.mul(offset));

				T t = (offset - n_dot_s) / n_dot_v;
				*distance = t;
				*norm = normal;

				return true;
			}

			return false;
		}
	};
	typedef Plane<float> FPlane;


	template <typename T> class Sphere : public Surface<T> {
	public:
		Vector3<T> origin;
		T radius;

	public:
		Sphere(Vector3<T>& o, T r) : origin(o), radius(r) {}

		bool intersect(Ray<T>& ray, T* distance, Vector3<T>* normal) {
			// Ray => S + tP = 0;
			// Sphere => x^2 + y^2 + z^2 = 0
			
			// Offset ray.
			Vector3<T> S = ray.origin.sub(origin);
			Vector3<T> P = ray.direction.normalize();

			T p_dot_s = P.dot(S);
			T s_squared = S.dot(S);

			T a = 1; // P is normalized, otherwise P.P
			T b = 2 * p_dot_s;
			T c = s_squared - (radius * radius);

			T d = (b * b) - (4 * a * c);

			if (d < 0) {
				return false;
			}
			else {
				T t = (-b - sqrt(d)) / (2 * a);
				*distance = t;
				Vector3<T> hitPoint = ray.origin.add(ray.direction.mul(t));
				*normal = hitPoint.sub(origin).normalize();
				return true;
			}
		}
	};
	typedef Sphere<float> FSphere;

}

#endif