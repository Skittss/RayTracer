#pragma once

#include "material.h"
#include "../data/colour.h"
#include "../data/vector.h"

class Normat : public Material {
public:

	Normat() {
        this->albedo = new BlockTexture(Colour(0.0f));
        this->specular_albedo = new BlockTexture(Colour(0.0f));
    }

    Colour evaluate_emissive(const Hit &hit) override {
		// Slightly hacky way of adding ambient colour while still adhering to the rendering equation.
        Colour normal = Colour(hit.normal.x, hit.normal.z, hit.normal.y);
		return normal * 0.5f + 0.5f;
	}

	Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override {
        return Colour(0.0f);
	}

    // TODO: This sample_brdf shouldn't really be called for a falsecolour material like normat, but just in case use
    //         Normal diffuse scattering.
    Colour sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &pdf) {
        
        Vector n_tangent, n_bitangent;
        UTIL::get_orthonormal_bases(hit.normal, n_tangent, n_bitangent);

        Vector wi_local = UTIL::world_to_local(wi, n_tangent, hit.normal, n_bitangent);
        Vector wr_local = UTIL::hemisphere_scatter(pdf);
        Colour fr = evaluate_brdf(hit, wi_local, wr_local);

        wr = UTIL::local_to_world(wr_local, n_tangent, hit.normal, n_bitangent);

        return fr;
    }
};
