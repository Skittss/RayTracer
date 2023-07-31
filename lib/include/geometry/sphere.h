#pragma once

#include "../data/vector.h"
#include "object3D.h"

class Sphere : public Object3D {
private:
	Vector center;
	float  radius;

	void get_uv(Vector pos, float &u, float &v);

public:
	Sphere(Vector c, float r);
	vector<Hit> intersection(Ray ray) override;
};
