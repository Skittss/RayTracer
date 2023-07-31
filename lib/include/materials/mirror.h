#pragma once

#include "global_material.h"
#include "../textures/block_texture.h"

class Mirror : public GlobalMaterial {

public:
    Mirror() : Mirror(Colour(1.0f)) {}

    Mirror(Colour specular_albedo) {
        this->specular_albedo = new BlockTexture(specular_albedo);
    }
    
    Mirror(Colour diffuse_albedo, Colour specular_albedo) {
        this->albedo = new BlockTexture(diffuse_albedo);
        this->specular_albedo = new BlockTexture(specular_albedo);
    }

    Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override {
        return specular_albedo->map_uv(hit.u, hit.v);
    }

    Colour sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &pdf) override {

        wr = reflect(wi, hit.normal); // No need to convert to local space here
        pdf = 1.0f; // Perfect mirror.

        return evaluate_brdf(hit, wi, wr);
    }

};