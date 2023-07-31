// Exit codes:
//    -1: Dimensions are <= 0 or exceed maximum value.
//    -2: Insufficient memory to init framebuffer.
//    -3: Out of bounds framebuffer access.
//    -4: Could not open file for writing.
//    -5: Unexpected file write error.

#pragma once

#include <stdlib.h>
#include <float.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "colour.h"

using namespace std;

typedef struct {
	float r;
	float g;
	float b;
	float d;
} Pixel;

class FrameBuffer {
private:
	int width;
	int height;
	Pixel *framebuffer;

	bool out_of_bounds(int x, int y) {
		return x < 0 || x >= width || y < 0 || y >= height;
	}

	void exit_if_out_of_bounds(int x, int y) {
		if (out_of_bounds(x, y)) {
			std::ostringstream err;
			err << "ERR: Out of bounds access of framebuffer. Dimensions: [" << width << "," << height << "]; Accessed: [" << x << "," << y << "]." << endl;
			throw std::logic_error(err.str());
		}
	}

	void get_fb_min_max_colour(float &minimum, float &maximum);
	void get_fb_min_max_depth(float &minimum, float &maximum);

	float lerp(float a, float b, float r);
	float clamp_colour_value(float c);
	float lerp_step(float x, float edge_start, float edge_end); // 0 -> 1 Step function lerped between two points.
	float sigmoid_step(float x, float edge_start, float edge_end); // 0 -> 1 Step with s-curve between two points.

	void apply_exposure(float &value, float e);
	void apply_contrast_and_brightness(float &value, float c, float b);
	void apply_colour_filter(float &r, float &g, float &b, Colour c);
	void apply_saturation(float &value, float gray, float s);
	void apply_GT_tone_map(float &value);
	void apply_ACES_tone_map(float &value);
	void apply_gamma(float &value, float g);

	void post_process(float e, float c, float b, Colour filter, float s, float g);
	void colour_correct(float diff, float contrast, float brightness, float saturation, float gamma);

	
public:
	FrameBuffer(int w, int h);

	void set_colour(int x, int y, float red, float green, float blue);
	void set_depth(int x, int y, float depth);
	void export_rgb(char *filename);
	void export_depth(char *filename);
};
