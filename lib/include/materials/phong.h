#pragma once

#include "../data/colour.h"
#include "../data/vector.h"

#include "../textures/texture.h"
#include "../textures/block_texture.h"

#include "material.h"

class Phong : public Material {
public:
	Texture *ambient;
	float k_a;	// k - constant determining each component contribution to final colour.
				//     i.e. this is ambient amount.

	float k_d; // Diffuse (albedo) coefficient.

	float k_s; // Specular coefficient.
	float  alpha;	// Responsible for specular highlight light attenuation.

	Phong() : Phong(
		Colour(1.0f, 1.0f, 1.0f), 1.0f,
		Colour(1.0f, 1.0f, 1.0f), 1.0f,
		Colour(1.0f, 1.0f, 1.0f), 1.0f,
		40.0f
	) {}

	Phong(Colour ambient, Colour diffuse, Colour specular, float alpha) : Phong(
		ambient, 1.0f,
		diffuse, 1.0f,
		specular, 1.0f,
		alpha
	) {}

	Phong(Colour ambient, float k_a, Colour diffuse, float k_d, Colour specular, float k_s, float alpha) {
		
		this->ambient = new BlockTexture(ambient);
		this->k_a = k_a;
		
		this->albedo = new BlockTexture(diffuse);
		this->k_d = k_d;

		this->specular_albedo = new BlockTexture(diffuse);
		this->k_s = k_s;
		this->alpha = alpha;
	}

	void apply_texture(Texture *texture) {
		albedo = texture;
	}

	Colour evaluate_emissive(const Hit &hit) override {
		// Slightly hacky way of adding ambient colour while still adhering to the rendering equation.
		return k_a * ambient->map_uv(hit.u, hit.v);
	}

	Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override;

	Colour sample_brdf(const Hit &hit, const Vector &wr, Vector &wi, float &pdf) override;
};
