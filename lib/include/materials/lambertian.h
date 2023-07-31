#pragma once

#include "../data/colour.h"
#include "../data/vector.h"

#include "../textures/texture.h"
#include "../textures/block_texture.h"

#include "material.h"

class Lambertian : public Material {
public:

	Lambertian() : Lambertian(
		Colour(1.0f),
		Colour(0.0f)
	) {}

	Lambertian(Colour albedo) : Lambertian(albedo, Colour(0.0f)) {}
	
	Lambertian(Colour diffuse_albedo, Colour specular_albedo) {
		this->albedo = new BlockTexture(diffuse_albedo);
		this->specular_albedo = new BlockTexture(specular_albedo);
	}

	void apply_texture(Texture *texture) {
		albedo = texture;
	}

	Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override {
		return albedo->map_uv(hit.u, hit.v) / M_PI; //TODO needs uv
	}

	Colour sample_brdf(const Hit &hit, const Vector &wr, Vector &wi, float &pdf) override;

};
