#pragma once

#include <vector>

#include "global_material.h"
#include "../textures/block_texture.h"

class Dielectric : public GlobalMaterial {

    float ior = 1.5f;

public:

    Dielectric(float ior) : Dielectric(ior, Colour(1.0f)) {}

    Dielectric(float ior, Colour albedo) {
        this->ior = ior;
        this->specular_albedo = new BlockTexture(albedo);    
    }

    Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) override {
        return specular_albedo->map_uv(hit.u, hit.v);
    }

    Colour sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &pdf) override {
        // throw std::logic_error("Tried to single-sample from dielectric material.");

        float reflectivity = schlick(wi, hit.normal, ior);

        if (Vector::dot_product(wi, hit.normal) > 0) {
            // On way out, transmit all photons.
            wr = refract(wi, hit.normal, ior);
        } else {
            // Proportion of reflected rays is proportional to the fresnel term for reflectivity.
            if (UTIL::rand_float(0.0f, 1.0f) < reflectivity) {
                // Reflect
                wr = reflect(wi, hit.normal);
            } else {
                // Refract
                wr = refract(wi, hit.normal, ior);
            }
        }
        pdf = 1.0f;

        return evaluate_brdf(hit, wi, wr);
    }

    // TODO: Negate wi for this specific implementation to be consistent with brdf wi.
    vector<std::tuple<Vector, Colour, TRANSMISSION_TYPE>> multi_sample_brdf(const Hit &hit, Vector wi) {
        vector<std::tuple<Vector, Colour, TRANSMISSION_TYPE>> pairs;

        // Incorporate fresnel factor to brdf evaluation.
        float reflectivity = schlick(wi, hit.normal, ior);
        float transmittance = 1.0f - reflectivity;

        if (wi.dot(hit.normal) > 0) {
            
            // Only sample refraction on the way out.
            Vector refract_wr = refract(wi, hit.normal, ior);
            if (!Vector::is_zero(refract_wr)) {
                pairs.emplace_back(std::tuple<Vector, Colour, TRANSMISSION_TYPE>(
                    refract_wr,
                    evaluate_brdf(hit, wi, refract_wr),
                    TRANSMISSION_TYPE::REFRACTION
                ));
            }

        } else {

            Vector reflect_wr = reflect(wi, hit.normal);
            pairs.emplace_back(std::tuple<Vector, Colour, TRANSMISSION_TYPE>(
                reflect_wr, 
                reflectivity * evaluate_brdf(hit, wi, reflect_wr),
                TRANSMISSION_TYPE::REFLECTION
            )); // Reflection

            Vector refract_wr = refract(wi, hit.normal, ior);
            if (!Vector::is_zero(refract_wr)) { // Don't refract if total internal reflection occurs.
                pairs.emplace_back(std::tuple<Vector, Colour, TRANSMISSION_TYPE>(
                    refract_wr,
                    transmittance * evaluate_brdf(hit, wi, refract_wr),
                    TRANSMISSION_TYPE::REFRACTION
                )); // Refraction
            }
        }

        return pairs;
    }

};