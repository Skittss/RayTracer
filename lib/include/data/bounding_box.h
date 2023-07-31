#pragma once

#include <float.h>

#include "ray.h"
#include "vector.h"

/// @brief A simple AABB bounding box.
class BoundingBox {

private:

    Vector pos;
    float x, y, z;

public:

    BoundingBox(Vector pos, float x, float y, float z) : pos(pos), x(x), y(y), z(z) {}

    bool in_bounds(Vector v) {
        Vector abs_l_pos = Vector::v_abs(v - pos); 
        return (abs_l_pos.x <= x && abs_l_pos.y <= y && abs_l_pos.z <= z);
    }

    bool intersect(Ray ray) {

        Vector local_ray_position = ray.position;
        Vector local_ray_dir = ray.direction;

        Vector half_bounds = Vector(x, y, z);
        Vector min_bound = pos - half_bounds;
        Vector max_bound = pos + half_bounds;
        Vector inv_dir = 1.0f / local_ray_dir; // TODO: care of div/0 here.

        Vector negatives = Vector(
            inv_dir.x < 0,
            inv_dir.y < 0,
            inv_dir.z < 0
        );


        Vector t_min = inv_dir * (Vector::v_lerp(max_bound, min_bound, negatives) - local_ray_position);
        Vector t_max = inv_dir * (Vector::v_lerp(min_bound, max_bound, negatives) - local_ray_position);

        // Check x-y corner miss edge cases
        if (t_min.x > t_max.y || t_min.y > t_max.x) return false;

        float t_near = t_min.x;
        float t_far  = t_max.x;
        
        if (t_min.y > t_min.x) t_near = t_min.y; // Note +ve y is downwards in this renderer's convention
        if (t_max.y < t_max.x) t_far  = t_max.y;

        // Check x (or y, whichever is the relevant intersection) - z miss edge cases
        if (t_near > t_max.z || t_min.z > t_far) return false;
        
        if (t_min.z > t_near) t_near = t_min.z;
        if (t_max.z < t_far)  t_far  = t_max.z;

        if (t_near <= 0.0f && t_far <= 0.0f) return false;

        return true;
    }
};