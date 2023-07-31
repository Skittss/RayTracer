#include <materials/phong.h>
#include <util/util.h>

#include <math.h>

Colour Phong::evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) {

	Colour fr = Colour(0.0f);

	fr += k_d * albedo->map_uv(hit.u, hit.v); // Albedo (Diffuse) component

	Vector r = Vector::norm(Vector::reflect(wi, hit.normal));
	float h = Vector::dot_product(r, wr);

	if (h > 0.0f) fr += k_s * specular_albedo->map_uv(hit.u, hit.v) * (float)pow(h, alpha); // Specular component

	return fr;
}

Colour Phong::sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &pdf) {
    
    Vector n_tangent, n_bitangent;
    UTIL::get_orthonormal_bases(hit.normal, n_tangent, n_bitangent);

    Vector wi_local = UTIL::world_to_local(wi, n_tangent, hit.normal, n_bitangent);
    Vector wr_local = UTIL::hemisphere_scatter(pdf);
    Colour fr = evaluate_brdf(hit, wi_local, wr_local);

    wr = UTIL::local_to_world(wr_local, n_tangent, hit.normal, n_bitangent);

    return fr;
}

