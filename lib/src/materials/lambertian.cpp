#include <materials/lambertian.h>

#include <util/util.h>

// TODO: Abstract world space conversion to functino in object3D not here.
Colour Lambertian::sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &pdf) {
    
    Vector n_tangent, n_bitangent;
    UTIL::get_orthonormal_bases(hit.normal, n_tangent, n_bitangent);

    Vector wi_local = UTIL::world_to_local(wi, n_tangent, hit.normal, n_bitangent);
    Vector wr_local = UTIL::hemisphere_scatter(pdf);
    Colour fr = evaluate_brdf(hit, wi_local, wr_local);

    wr = UTIL::local_to_world(wr_local, n_tangent, hit.normal, n_bitangent);

    return fr;
}