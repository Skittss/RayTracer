#pragma once

#include "texture.h"

class BlockTexture : public Texture {
public:

    Colour colour;
    
    BlockTexture(Colour col) : colour(col) {}

    Colour map_uv(float u, float v) {
        return colour;
    }
};