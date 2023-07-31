#pragma once

#include "light.h"

#include "../util/util.h"
#include "../materials/emissive.h"

class SquareLight : public Light, public Plane {
public:

	float span; // width / 2.
	float attenuation_radius; // Light attenuation factor for use in whitted raytracing only.
    Vector up;

	Vector memo_tangent;
	Vector memo_bitangent;
	bool needs_memo = true;

	SquareLight(Vector pos, Vector up, Colour col, float i, float r) : Light(), Plane(pos, r, up) {
		Light::position = pos;
        this->up = up;
		colour = col;
		intensity = i;
		span = r;
		material = new EmissiveMaterial(col * i);
		constant_sample = false;
	}

	SquareLight(
		Vector pos, Vector up, 
		Colour col, float i, 
		float r, float whitted_attenuation_radius
	) : SquareLight(pos, up, col, i, r) {
		attenuation_radius = whitted_attenuation_radius;
	}

	bool get_direction(const Vector &surface_pt, const Vector &surface_norm, Vector &out_dir) override {
		out_dir = surface_pt - Light::position;
		out_dir.normalise();

		return up.dot(out_dir) > 0;
	}

	Colour get_intensity(const Vector &surface_pt) override {
		float distsq = (surface_pt - Light::position).length_squared();

    	return attenuation_radius * intensity * colour / distsq; // TODO.
	}

	Vector sample_position(float &pdf) override {
		pdf = 1.0f / (span * span * 4); // 1 / Surface Area.
		
		float u = UTIL::rand_float(-1.0f, 1.0f) * span;
        float v = UTIL::rand_float(-1.0f, 1.0f) * span;

		Vector t, bt;
        UTIL::get_orthonormal_bases(up, t, bt); // TODO: could store this as a member var to avoid recalculation.

		Vector test = u * t + v * bt;
		Vector test2 = Light::position + test;

		return Light::position + u * t + v * bt;
	}

	float dot_with_normal(Vector o) override {
		return o.dot(up);
	}

	void generate_photon(Vector &photon_origin, Vector &photon_dir) override {

		if (needs_memo) UTIL::get_orthonormal_bases(up, memo_tangent, memo_bitangent);

		float _;
		Vector wr_local = UTIL::hemisphere_scatter(_);

		photon_origin = sample_position(_);
		photon_dir = UTIL::local_to_world(wr_local, memo_tangent, up, memo_bitangent);
	}
};
