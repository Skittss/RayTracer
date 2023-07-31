#pragma once

#include "texture.h"

class CheckerTexture : public Texture {
public:

    float c_width;
    float c_height;
    Colour even_colour;
    Colour odd_colour;

    CheckerTexture() : CheckerTexture(
        Colour(1.0f, 1.0f, 1.0f),
        Colour(0.0f, 0.0f, 0.0f),
        1.0f, 1.0f
    ) {}

    CheckerTexture(Colour even_col, Colour odd_col) : CheckerTexture() {
        even_colour = even_col;
        odd_colour  = odd_col;
    }

    CheckerTexture(float checker_width, float checker_height) : CheckerTexture() {
        c_width  = checker_width;
        c_height = checker_height;
    }

    CheckerTexture(Colour even_col, Colour odd_col, float checker_width, float checker_height) :
        even_colour(even_col),
        odd_colour(odd_col),
        c_width(checker_width),
        c_height(checker_height)
    {}

    Colour map_uv(float u, float v) {
        int x = floorf(u * c_width);
        int y = floorf(v * c_height);
        
        if ((x + y) % 2 == 0) return even_colour;
        return odd_colour;
    }
};