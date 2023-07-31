#include <geometry/cube.h>
#include <util/util.h>

Cuboid::Cuboid(Vector pos, Vector up, float width_n, float width_ta, float width_bt) {
    this->pos = pos;
    
    this->up = Vector::norm(up);
    UTIL::get_orthonormal_bases(this->up, tangent, bitangent);

    this->width_n = width_n / 2.0f;
    this->width_ta = width_ta / 2.0f;
    this->width_bt = width_bt / 2.0f;
}

Cuboid::Cuboid(Vector pos, Vector up, Vector right, float width_n, float width_ta, float width_bt) {
    this->pos = pos;
    this->up = Vector::norm(up);

    UTIL::get_orthonormal_bases(this->up, right, tangent, bitangent);

    this->width_n = width_n / 2.0f;
    this->width_ta = width_ta / 2.0f;
    this->width_bt = width_bt / 2.0f;
}

void Cuboid::get_uv(Vector pos, float &u, float &v) {
    // TODO:: Could implement cube-mapping function here: for now just use 0 for u-v;
    u = 0; v = 0;
}

Vector Cuboid::get_local_normal(Vector local_pos, Vector min_bound, Vector max_bound) {

    local_pos = UTIL::world_to_local(local_pos, tangent, up, bitangent);

    Vector c = Vector::v_abs(local_pos - min_bound);
    Vector f = Vector::v_abs(local_pos - max_bound);

    float epsilon = 0.001f;
    Vector out = Vector(0.0f);

    if (c.x < epsilon) out = Vector(-1.0, 0.0, 0.0);
    else if (f.x < epsilon) out = Vector(1.0, 0.0, 0.0);
    else if (c.y < epsilon) out = Vector(0.0, -1.0, 0.0);
    else if (f.y < epsilon) out = Vector(0.0, 1.0, 0.0);
    else if (c.z < epsilon) out = Vector(0.0, 0.0, -1.0);
    else if (f.z < epsilon) out = Vector(0.0, 0.0, 1.0);

    return UTIL::local_to_world(out, tangent, up, bitangent);
}

vector<Hit> Cuboid::intersection(Ray ray) {
    
    vector<Hit> hits;

    Vector local_ray_position = UTIL::world_to_local(ray.position, tangent, up, bitangent);
    Vector local_ray_dir = UTIL::world_to_local(ray.direction, tangent, up, bitangent);

    Vector half_bounds = Vector(width_ta, width_n, width_bt);
    Vector min_bound = pos - half_bounds;
    Vector max_bound = pos + half_bounds;
    Vector inv_dir = 1.0f / local_ray_dir; // TODO: care of div/0 here.

    // AABB Intersection in cuboid-local space.
    //   See https://www.rose-hulman.edu/class/csse/csse451/AABB/ for further explanation.
    //   This could be sped up slightly by moving calculation of normals here and by only considering z in x,y failure case.

    Vector negatives = Vector(
        inv_dir.x < 0,
        inv_dir.y < 0,
        inv_dir.z < 0
    );

    Vector t_min = inv_dir * (Vector::v_lerp(max_bound, min_bound, negatives) - local_ray_position);
    Vector t_max = inv_dir * (Vector::v_lerp(min_bound, max_bound, negatives) - local_ray_position);

    // Check x-y corner miss edge cases
    if (t_min.x > t_max.y || t_min.y > t_max.x) return hits;

    float t_near = t_min.x;
    float t_far  = t_max.x;
    Vector min_normal = Vector(-1.0f, 0.0f, 0.0f);
    Vector max_normal = Vector(1.0f, 0.0f, 0.0f);
    
    if (t_min.y > t_min.x) {t_near = t_min.y; min_normal = Vector(0.0f, 1.0f, 0.0f);} // Note +ve y is downwards in this renderer's convention
    if (t_max.y < t_max.x) {t_far  = t_max.y; max_normal = Vector(0.0f, -1.0f, 0.0f);}

    // Check x (or y, whichever is the relevant intersection) - z miss edge cases
    if (t_near > t_max.z || t_min.z > t_far) return hits;
    
    if (t_min.z > t_near) {t_near = t_min.z; min_normal = Vector(0.0f, 0.0f, -1.0f);}
    if (t_max.z < t_far)  {t_far  = t_max.z; max_normal = Vector(0.0f, 0.0f, 1.0f);}


    Hit near_hit;
    near_hit.obj = this;
    near_hit.t = t_near;
    // Don't need to convert coord space here as t is just a linear distnace from ray origin.
    near_hit.position = ray.position + t_near * ray.direction; 
    get_uv(near_hit.position, near_hit.u, near_hit.v);
    near_hit.normal = get_local_normal(near_hit.position, min_bound, max_bound);
    near_hit.entering = true;
            
    hits.emplace_back(near_hit);
    

    Hit far_hit;
    far_hit.obj = this;
    far_hit.t = t_far;
    far_hit.position = ray.position + t_far * ray.direction;
    get_uv(far_hit.position, far_hit.u, far_hit.v);
    far_hit.normal = get_local_normal(far_hit.position, min_bound, max_bound);
    far_hit.entering = false;

    hits.emplace_back(far_hit);

    return hits;
}