#include <geometry/plane.h>
#include <util/util.h>

#include <vector>

Plane::Plane(Vector center, float tangent_size, float bitangent_size, Vector up) {
    this->center = center;
    this->tangent_size = tangent_size / 2.0f;
    this->bitangent_size = bitangent_size / 2.0f;

    normal = Vector::norm(up);

    UTIL::get_orthonormal_bases(normal, tangent, bitangent);
}

Plane::Plane(Vector center, float size, Vector up) : Plane(center, size, size, up) {}

Plane::Plane(Vector center, float tangent_size, float bitangent_size, Vector up, Vector right) {
    this->center = center;
    this->tangent_size = tangent_size / 2.0f;
    this->bitangent_size = bitangent_size / 2.0f;
    
    up.normalise();
    normal = up;
    UTIL::get_orthonormal_bases(up, right, tangent, bitangent);
}

Plane::Plane(Vector center, float size, Vector up, Vector right) : Plane(center, size, size, up, right) {}

void Plane::get_uv(Vector pos, float &u, float &v) {
    u = tangent.dot(pos);
    v = bitangent.dot(pos);
}

vector<Hit> Plane::intersection(Ray ray) {
    
    vector<Hit> hits;

    Vector ro = ray.position - center;
    float dot_product = ro.dot(normal);
    if (dot_product == 0) return hits;
    if (dot_product < 0 && !twoside) return hits;

    float denominator = ray.direction.dot(normal);
    if (fabs(denominator) < 0.0001f) return hits;

    float t = -dot_product / denominator; // Unbounded hit on surface plane t

    // Calculate against 3D Plane equation: c + u x d1 + v x d2
    Vector hit_pos = ray.position + t * ray.direction;

    Vector center_offset = Vector::v_abs(hit_pos - center);
    float ortho_1 = fabs(tangent.dot(center_offset));
    float ortho_2 = fabs(bitangent.dot(center_offset));

    if (ortho_1 < tangent_size && ortho_2 < bitangent_size) {
        Hit hit;
        hit.obj = this;
        hit.t = t;
        hit.position = ray.position + t * ray.direction;
        
        if (normal.dot(ray.direction) > 0.0f) hit.normal = -normal;
        else hit.normal = normal;
        hit.entering = true;
        get_uv(hit.position, hit.u, hit.v);
        
        hits.emplace_back(hit);
    }


    return hits;
}