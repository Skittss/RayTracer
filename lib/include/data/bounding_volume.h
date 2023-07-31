#pragma once

#include <float.h>

#include "ray.h"
#include "../geometry/polymesh.h"

class PolyMesh;

class BoundingVolume {

private:
    // Length of oblique plane normals (calculate once to avoid lots of sqrts)
    const float o_len = sqrtf(3) / 3.0f; 
    // Normals of 7 base planes which make up the bounding volumes.
    const Vector plane_normals[7] = {
        Vector(1, 0, 0), // Primary Axes
        Vector(0, 1, 0),
        Vector(0, 0, 1),
        Vector( o_len,  o_len, o_len), // Diagonals
        Vector(-o_len,  o_len, o_len),
        Vector(-o_len, -o_len, o_len),
        Vector( o_len, -o_len, o_len)
    };

    float d[7][2];

public:

    BoundingVolume(PolyMesh *mesh);
    bool intersect(Ray ray); 
};