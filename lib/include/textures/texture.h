#pragma once

#include "../data/colour.h"

class Texture {
public:
    virtual Colour map_uv(float u, float v) = 0;
};