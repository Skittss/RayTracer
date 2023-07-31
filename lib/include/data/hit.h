#pragma once

#include <iostream>
#include "vector.h"

// Declare Obj3d here to avoid circular include on build.

class Object3D;

class Hit {
public:

	float t; // Coefficient of ray direction - 'distance' of intersection from ray cast origin. I.e. depth.
	Object3D *obj;

	Vector position;
	Vector normal;
	bool entering;

	float u;
	float v;
};
