#pragma once

#include "object3D.h"

class Plane : public Object3D {
private:

    Vector tangent;
    Vector bitangent;
    Vector normal;

    void get_uv(Vector pos, float &u, float &v);

public:

    Vector center; // TODO: replace with Object position attrib.
    float tangent_size;
    float bitangent_size;
    bool twoside = true;

    Plane(Vector center, float size, Vector up);
    Plane(Vector center, float tangent_size, float bitangent_size, Vector up);

    Plane(Vector center, float size, Vector up, Vector right);
    Plane(Vector center, float tangent_size, float bitangent_size, Vector up, Vector right);

    vector<Hit> intersection(Ray ray) override;
};