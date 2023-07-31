#pragma once

#include "material.h"
#include "../textures/block_texture.h"

class EmissiveMaterial : public Material {

public:

    Colour light_flux;

    EmissiveMaterial() : EmissiveMaterial(Colour(1.0f)) {}

    EmissiveMaterial(Colour albedo) : Material(MATERIAL_TYPE::EMISSIVE) {
        this->albedo = new BlockTexture(albedo);
        light_flux = albedo;
    }

    virtual Colour evaluate_emissive(const Hit &hit) {
		return light_flux;
	}

    Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override {
        return albedo->map_uv(hit.u, hit.v);
    }

};