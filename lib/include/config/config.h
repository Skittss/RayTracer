#pragma once

#include "../data/colour.h"

namespace CONFIG {

// Camera config
constexpr int WIDTH = 2048;
constexpr int HEIGHT = 2048;
constexpr int MAX_WIDTH = 2048;
constexpr int MAX_HEIGHT = 2048;
constexpr int CAMERA_FOV = 50;
constexpr float CAMERA_FOCAL_LENGTH = 1;

// Max No. of light bounces.
constexpr int MAX_RECURSION_DEPTH = 8;

// Number of raycasts averaged per pixel to employ anti-aliasing via supersampling.
constexpr int SSAA_SAMPLES = 1;

// PHOTON-MAPPING ONLY: Number of photons.
constexpr int NUM_GLOBAL_PHOTONS = 1000000;
constexpr int NUM_CAUSTIC_PHOTONS = 20000000;
constexpr int PHOTONS_PER_ESTIMATE = 500;

constexpr int PENUMBRA_SAMPLES = 200;

// Post Processing

    constexpr float EXPOSURE = 1.0f;
    constexpr float COLOUR_FILTER_R = 1.0f;
    constexpr float COLOUR_FILTER_G = 1.0f;
    constexpr float COLOUR_FILTER_B = 1.0f;

    // Colour Correction
    constexpr float CONTRAST = 1.0f;
    constexpr float BRIGHTNESS = 0.0f;
    constexpr float SATURATION = 1.0f;
    constexpr float GAMMA = 1.0f;

// Note: Paths are one level up as cmake builds to ./build not ./
// Import paths
constexpr char *TEAPOT_PATH = (char *)"../src/models/teapot_smaller.ply";

// Exoport paths
constexpr char *RENDER_OUTPUT_PATH = (char *)"../exports/ppm/render.ppm";
constexpr char *DEPTH_OUTPUT_PATH = (char *)"../exports/ppm/depth.ppm";

}