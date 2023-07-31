#pragma once

#include "material.h"
#include "util/util.h"

class GlobalMaterial : public Material {

public:

    GlobalMaterial() : Material(MATERIAL_TYPE::SPECULAR) {
        albedo = new BlockTexture(Colour(0.0f));
        specular_albedo = new BlockTexture(Colour(1.0f));
    }

    Vector reflect(Vector wi, Vector n) {
        // Determine where to reflect the incident ray to sing law of reflection: R = I - 2(N . I)N
        return wi - 2.0f * n.dot(wi) * n;
    }

    Vector refract(Vector wi, Vector n, float ior) {
        // // Determine refracted ray derived from snell's law R = eta * I + (eta * c1 - c2) * N

        float n_dot_i = Vector::dot_product(wi, n);
        float cos_i = UTIL::clamp(n_dot_i, -1, 1); 
        float eta1 = 1; // IOR of air - assume wi origin is in air.
        float eta2 = ior; // IOR of dielectric material.
        float eta;	// Ratio of index of refractions of the two materials travelling between.

        Vector facing_refract_norm = n; 

        // In -> Out surface
        if (cos_i < 0) { 
            cos_i = -cos_i;
            eta = eta1 / eta2;
            
        // Out -> In surface
        } else { 
            // Invert normal to ensure facing the same way as refraction direction
            facing_refract_norm = -n;
            eta = eta2 / eta1;
        } 

        // Account for total internal reflection, i.e. eta ^ 2 * (1 - cos^2 i) is negative.
        //  Denoted by k; this is the term inside the sqrt of c2 from Snell's law.
        float k = 1 - eta * eta * (1 - cos_i * cos_i); 
        if (k < 0) return Vector(0.0f);

        // Refracted Ray.
        return eta * wi + (eta * cos_i - sqrtf(k)) * facing_refract_norm; // Snell's law
    }
    
    float fresnel(Vector wi, Vector n, float ior) {
        float cos_theta1 = UTIL::clamp(wi.dot(n), -1, 1);

        float eta;
        float eta1 = ior;
        float eta2 = 1; // Assume other medium is air.

        if (cos_theta1 < 0) eta = ior / eta2;
        else eta = eta2 / ior;

        // Sin Theta via Snell's law
        float sin_theta = eta2 / eta1 * sqrtf(std::max(0.0f, 1.0f - cos_theta1 * cos_theta1));

        if (sin_theta >= 1) return 1;
        else {
            // More Snell's law
            float cos_theta2 = sqrt(std::max(0.0f, 1.0f - sin_theta * sin_theta));
            cos_theta1 = fabs(cos_theta1);
            float rs = ((eta2 * cos_theta1) - (eta1 * cos_theta2)) / ((eta2 * cos_theta1) + (eta1 * cos_theta2));
            float rp = ((eta1 * cos_theta2) - (eta2 * cos_theta1)) / ((eta1 * cos_theta2) + (eta2 * cos_theta1));
            return (rs * rs + rp * rp) / 2.0f;
        }
    }

    /// @brief Approximation of `GlobalMaterial::fresnel()`. Generally preferrable as faster and approximates very well.
    float schlick(Vector wi, Vector n, float ior) {
        float ratio = Vector::dot_product(wi, n) > 0.0f ? ior : 1.0f / ior; // Again assume other medium is air.
        float cost = std::max(0.0f, Vector::dot_product(-Vector::norm(wi), Vector::norm(n)));
        
        float sqrt_r0 = (1 - ratio) / (1 + ratio);
        float r0 = sqrt_r0 * sqrt_r0;
        float approx = r0 + (1 - r0) * pow(1 - cost, 5);

        return r0 + (1 - r0) * pow(1 - cost, 5);
    }

};