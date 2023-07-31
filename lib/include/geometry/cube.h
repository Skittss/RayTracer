#pragma once

#include "object3D.h"
#include "plane.h"

class Cuboid : public Object3D {

private:

    Vector up;
    Vector tangent; // Right
    Vector bitangent;

    Vector get_local_normal(Vector local_pos, Vector min_bound, Vector max_bound);
    void get_uv(Vector pos, float &u, float &v);

public:

    Vector pos;
    float width_n;
    float width_ta;
    float width_bt;

    Cuboid(Vector pos, Vector up, float width) : Cuboid(pos, up, width, width, width) {}
    Cuboid(Vector pos, Vector up, float width_n, float width_ta, float width_bt);

    Cuboid(Vector pos, Vector up, Vector right, float width) : Cuboid(pos, up, right, width, width, width) {}
    Cuboid(Vector pos, Vector up, Vector right, float width_n, float width_ta, float width_bt);
    
    vector<Hit> intersection(Ray ray) override;

};