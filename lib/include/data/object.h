#pragma once

#include "vector.h"
#include "transform.h"

class Object {

public:
    Vector position;

    Object() {
        position = Vector(0.0f, 0.0f, 0.0f);
    }

    Object(Vector pos) : position(pos) {}

};