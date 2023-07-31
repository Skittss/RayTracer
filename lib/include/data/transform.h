#pragma once

#include "vector.h"

class Transform {
public:
  
	float matrix[4][4];

	Transform() : Transform(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	) {}

	Transform(
		float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p
	) {
		matrix[0][0] = a; matrix[0][1] = b; matrix[0][2] = c; matrix[0][3] = d;
		matrix[1][0] = e; matrix[1][1] = f; matrix[1][2] = g; matrix[1][3] = h;
		matrix[2][0] = i; matrix[2][1] = j; matrix[2][2] = k; matrix[2][3] = l;
		matrix[3][0] = m; matrix[3][1] = n; matrix[3][2] = o; matrix[3][3] = p;
	};

	static Transform identity() {
		return Transform(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	float get_scale_factor() const {
		return matrix[3][3];
	}

	// Apply shorthand as fourth row is basically useless for a simple euclidian transform.
	void apply_homogeneous(Vector& v) const {
		float x, y, z;

		x =   matrix[0][0] * v.x
			+ matrix[0][1] * v.y
			+ matrix[0][2] * v.z
			+ matrix[0][3];

		y =   matrix[1][0] * v.x
			+ matrix[1][1] * v.y
			+ matrix[1][2] * v.z
			+ matrix[1][3];

		z =   matrix[2][0] * v.x
			+ matrix[2][1] * v.y
			+ matrix[2][2] * v.z
			+ matrix[2][3];

		v.x = x; v.y = y; v.z = z;
	}

	void apply_homogeneous(const Vector& in, Vector& out) const {
		out.x =   matrix[0][0] * in.x
				+ matrix[0][1] * in.y
				+ matrix[0][2] * in.z
				+ matrix[0][3];

		out.y =   matrix[1][0] * in.x
				+ matrix[1][1] * in.y
				+ matrix[1][2] * in.z
				+ matrix[1][3];

		out.z =   matrix[2][0] * in.x
				+ matrix[2][1] * in.y
				+ matrix[2][2] * in.z
				+ matrix[2][3];
	}
};
