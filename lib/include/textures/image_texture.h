#pragma once

#include <iostream>
#include "texture.h"

class ImageTexture : public Texture {
public:

    ImageTexture(char *file_path) {
        std::cerr << "Not implemented: ImageTexture constructor from file.";
    }

    Colour map_uv(float u, float v) {
        std::cerr << "Not implemented: ImageTexture map uv.";
        return Colour(0.0f, 0.0f, 0.0f);
    }
};