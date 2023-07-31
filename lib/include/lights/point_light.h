#pragma once

#include "light.h"

#include "../util/util.h"

class PointLight : public Light {
public:

	float radius;

	PointLight() : Light() {}

	PointLight(Vector pos, Colour col, float i, float r) : Light() {
		position = pos;
		colour = col;
		intensity = i;
		radius = r;
	}

	bool get_direction(const Vector &surface_pt, const Vector &surface_norm, Vector &out_dir) override {
		out_dir = surface_pt - position;
		out_dir.normalise();

		return true; // Point light has no normal.
	}

	Colour get_intensity(const Vector &surface_pt) override {
		float distsq = (surface_pt - position).length_squared();
		
		return radius * colour * intensity / distsq;
	}

	Vector sample_position(float &pdf) override {
		pdf = 1.0f;
		return position;
	}

	float dot_with_normal(Vector o) override {
		return 1.0f;
	}

	void generate_photon(Vector &photon_origin, Vector &photon_dir) override {
		photon_origin = position;
		photon_dir = Vector(
			UTIL::rand_float(-1.0f, 1.0f),
			UTIL::rand_float(-1.0f, 1.0f),
			UTIL::rand_float(-1.0f, 1.0f)
		);
	}

};
