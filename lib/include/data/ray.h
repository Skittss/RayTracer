#pragma once

#include "vector.h"

class Ray {
public:
	Vector position;
	Vector direction;

	Ray() {}

	Ray(Vector pos, Vector dir) : position(pos), direction(dir) {}
	
};
