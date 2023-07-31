#include <util/util.h>

#include <stdlib.h>
#include <algorithm>
#include <iostream>

void UTIL::display_progress(int total, int i, char *counter, char *label) {
	
	float current = i;
	float total_f = total;
	float progress = current / total_f;
	const int bar_width = 70;

	std::cerr << "  [";
	int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cerr << "=";
        else if (i == pos) std::cerr << ">";
        else std::cerr << " ";
    }
	std::cerr << "] " << int(progress * 100.0) << " % (" << current << "/" << total << " " << counter << ") " << label << "\r";
    std::cerr.flush();
}

void UTIL::display_render_progress(int dim_x, int dim_y, int x, int y) {
	
	float current = dim_x * y + x;
	float total = dim_x * dim_y;
	float progress = current / total;
	const int bar_width = 70;

	cerr << "[";
	int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) cerr << "=";
        else if (i == pos) cerr << ">";
        else cerr << " ";
    }
	cerr << "] " << int(progress * 100.0) << " % (" << current << "/" << total << "px)\r";
    cerr.flush();
}

void UTIL::chrono_total_time_split(const int &total, int &s, int &m, int &h) {
    m = total / 60;
	s = total % 60;
	h = m / 60;
	m = m % 60;
}

float UTIL::clamp(float n, float min, float max) {
    return std::max(min, std::min(n, max));
}

float UTIL::lerp(float a, float b, float r) {
    return a * r + (1 - r) * b;
}

float UTIL::rand_float(float min, float max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

/// @brief Gaussian filter for photon mapping
/// @param d Distance from photon to intersection point
/// @param r Radiance estimate radius
/// @return w_pg
float UTIL::gaussian_filter(float d2, float r2) {
    float a = 1.818f;
    float b = 1.953f;

    return a * (1 - ((1 - exp(-b * d2 / (2 * r2))) / (1 - exp(-b))));
}

void UTIL::small_offset_ray(Ray &ray) {
    UTIL::small_offset_ray(ray, ray.direction);
}

void UTIL::small_offset_ray(Ray &ray, Vector bias) {
    ray.position += 0.001f * bias;
}

/// @brief Unbiased orthonormals calculation
void UTIL::get_orthonormal_bases(Vector normal, Vector &tangent, Vector &bitangent) {
    Vector a = Vector::cross_product(normal, Vector(1, 0, 0));
    Vector b = Vector::cross_product(normal, Vector(0, 1, 0));

    Vector max_a_b = a.dot(a) > b.dot(b) ? a : b;
    
    Vector c = Vector::cross_product(normal, Vector(0, 0, 1));

    Vector maximum = max_a_b.dot(max_a_b) > c.dot(c) ? max_a_b : c;
    maximum.normalise();
    
    tangent = maximum;
    bitangent = -Vector::cross_product(normal, tangent); // Bias +ve direction in z to make default orthobasis +ve in all dims.
}

/// @brief Biased orthonormals calculation.
/// @param right Direction to project tangent onto.
void UTIL::get_orthonormal_bases(Vector normal, Vector right, Vector &tangent, Vector &bitangent) {
    UTIL::get_orthonormal_bases(normal, tangent, bitangent);

    tangent = right - right.dot(normal) * normal;  // Project right onto orthogonal plane to find orthonormal basis in direction of r.
    tangent.normalise();
    bitangent = Vector::cross_product(tangent, normal);
}

Vector UTIL::world_to_local(Vector v, Vector local_ortho_x, Vector local_ortho_y, Vector local_ortho_z) {
    return Vector(
        v.dot(local_ortho_x),
        v.dot(local_ortho_y), 
        v.dot(local_ortho_z)
    );
}

Vector UTIL::local_to_world(Vector v, Vector local_ortho_x, Vector local_ortho_y, Vector local_ortho_z) {
    // Shorthand affine matrix calculation
    Vector world;
    for (int i = 0; i < 3; i++) {
        world[i] =  v[0] * local_ortho_x[i] +
                    v[1] * local_ortho_y[i] + 
                    v[2] * local_ortho_z[i];
    }
    return world;
}

Vector UTIL::spherical_to_cartesian(float theta, float phi) {
    return Vector(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));
}

Vector UTIL::hemisphere_scatter(float &pdf) {
    // Scatter randomly across a hemisphere in tangent space - MAKE SURE TO CONVERT RESULT BACK TO WORLD SPACE.
    const float u = UTIL::rand_float(0.0f, 0.999999f);
    const float v = UTIL::rand_float(0.0f, 0.999999f);
    const float theta = 0.5f * acos(UTIL::clamp(1.0f - 2.0f * u, -1.0f, 1.0f));
    const float phi   = 2 * M_PI * v;
    const float cost  = cos(theta);
    pdf = cost / M_PI;

    return UTIL::spherical_to_cartesian(theta, phi);
}