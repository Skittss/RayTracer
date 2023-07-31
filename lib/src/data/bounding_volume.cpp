#include <data/bounding_volume.h>

BoundingVolume::BoundingVolume(PolyMesh *mesh) {
    for (uint8_t i = 0; i < 7; i++)  {
        d[i][0] = FLT_MAX, d[i][1] = -FLT_MAX;
        mesh->compute_bounds(plane_normals[i], d[i][0], d[i][1]);
    }
}

bool BoundingVolume::intersect(Ray ray) {

    float nearest = -FLT_MAX; float farthest = FLT_MAX;

    for (uint8_t i = 0; i < 7; i++) {

        // TODO: num/den calculation could be batched if a full bounding volume hierarchy were implemented.
        float numerator   = Vector::dot_product(plane_normals[i], ray.position);
        float denominator = Vector::dot_product(plane_normals[i], ray.direction);

        // Check if ray intersects with slab.
        float near = (d[i][0] - numerator) / denominator;
        float far = (d[i][1] - numerator) / denominator;
        if (denominator < 0) std::swap(near, far);
        if (near > nearest) nearest = near;
        if (far < farthest) farthest = far;
        if (nearest > farthest) return false;
    }

    return true;
}