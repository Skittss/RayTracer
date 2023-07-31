#include <config/config.h>
#include <data/framebuffer.h>
#include <data/vector.h>
#include <util/util.h>

#include <iomanip>

FrameBuffer::FrameBuffer(int w, int h) {
	if (w <= 0 || w > CONFIG::MAX_WIDTH || h <= 0 || h > CONFIG::MAX_HEIGHT) {
		std::ostringstream err;
		err << "ERR: Could not init framebuffer with dimensions [" << w << "," << h << "]. Dimensions are <= 0 or exceed maximum value." << endl;
		std::logic_error(err.str());
	}

	width = w;
	height = h;
	framebuffer = new Pixel[w * h];

	// Ensure sufficient memory.
	if (!framebuffer) {
		std::ostringstream err;
		err << "ERR: Could not init framebuffer - Insufficient Memory." << endl;
		std::logic_error(err.str());
	}

	// Init empty framebuffy as all zeroes.
	for (int i = 0; i < w * h; i += 1) {
		framebuffer[i].r = 0.0f;
		framebuffer[i].g = 0.0f;
		framebuffer[i].b = 0.0f;
		framebuffer[i].d = 0.0f;
	}
}

float FrameBuffer::lerp(float a, float b, float r) {
	return a * r + (1 - r) * b;
}

float FrameBuffer::clamp_colour_value(float c) {
	return std::max(0.0f, std::min(c, 1.0f));
}

float FrameBuffer::lerp_step(float x, float edge_start, float edge_end) {
	if (x <= edge_start) return 0.0f;
	if (x >= edge_end)   return 1.0f;
	return (x - edge_start) / (edge_end - edge_start);
}

float FrameBuffer::sigmoid_step(float x, float edge_start, float edge_end) {
	if (x <= edge_start) return 0.0f;
	if (x >= edge_end)   return 1.0f;
	float edge_len = edge_end - edge_start;
	float dist_along_edge = x - edge_start;
	float slope_to_x = dist_along_edge / edge_len;
	return slope_to_x * slope_to_x * (3 - 2 * slope_to_x);
}

void FrameBuffer::apply_exposure(float &value, float e) {
	value = std::max(0.0f, value * e);
}

void FrameBuffer::apply_contrast_and_brightness(float &value, float c, float b) {
	value = std::max(0.0f, c * (value - 0.5f) + 0.5f + b);
}

void FrameBuffer::apply_colour_filter(float &r, float &g, float &b, Colour c) {
	r *= c.r;
	g *= c.g;
	b *= c.b;
}

void FrameBuffer::apply_saturation(float &value, float gray, float s) {
	value = std::max(0.0f, lerp(value, gray, s));
}

void FrameBuffer::apply_GT_tone_map(float &value) {
	// Curve sourced from "Hajime Uchimura's HDR Theory and Practice".
	// https://www.desmos.com/calculator/gslcdxvipg
	const float P = 1.0f;
	const float a = 1.0f;
	const float m = 0.22f;
	const float l = 0.4f;
	const float c = 1.33f;
	const float b = 0.0f;

	float l0 = ((P - m) * l) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = -C2 / P;

    float w0 = 1.0f - sigmoid_step(value, 0.0f, m);
    float w2 = lerp_step(value, m + l0, m + l0);
    float w1 = 1.0f - w0 - w2;

    float T = m * pow(value / m, c) + b;
    float S = P - (P - S1) * exp(CP * (value - S0));
    float L = m + a * (value - m);

    value = clamp_colour_value(T * w0 + L * w1 + S * w2);
}

void FrameBuffer::apply_ACES_tone_map(float &value) {
	// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
	value *= 0.6;
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    value = clamp_colour_value((value * (a * value + b)) / (value * (c * value + d) + e));
}

void FrameBuffer::apply_gamma(float &value, float g) {
	value = clamp_colour_value(pow(value, g));
}

void FrameBuffer::post_process(float e, float c, float b, Colour filter, float s, float g) {

	Vector grayscale_vec = Vector(0.299f, 0.587f, 0.114f); // BT 601 encoding.

	for (int i = 0; i < width * height; i++) {

		// Exposure
		apply_exposure(framebuffer[i].r, e);
		apply_exposure(framebuffer[i].g, e);
		apply_exposure(framebuffer[i].b, e);

		// TODO Could implement White balance here.

		// Contrast & Brightness
		apply_contrast_and_brightness(framebuffer[i].r, c, b);
		apply_contrast_and_brightness(framebuffer[i].g, c, b);
		apply_contrast_and_brightness(framebuffer[i].b, c, b);

		// Colour Filter
		apply_colour_filter(framebuffer[i].r, framebuffer[i].g, framebuffer[i].b, filter);

		// Saturation
		Vector col = Vector(framebuffer[i].r, framebuffer[i].g, framebuffer[i].b);
		float grayscale = Vector::dot_product(grayscale_vec, col);

		apply_saturation(framebuffer[i].r, grayscale, s);
		apply_saturation(framebuffer[i].g, grayscale, s);
		apply_saturation(framebuffer[i].b, grayscale, s);

		// ToneMap - HDR support, conversion from HDR -> LDR.
		apply_GT_tone_map(framebuffer[i].r);
		apply_GT_tone_map(framebuffer[i].g);
		apply_GT_tone_map(framebuffer[i].b);

		// Gamma
		apply_gamma(framebuffer[i].r, g);
		apply_gamma(framebuffer[i].g, g);
		apply_gamma(framebuffer[i].b, g);
	}
}

void FrameBuffer::set_colour(int x, int y, float red, float green, float blue) {
	exit_if_out_of_bounds(x, y);

	framebuffer[y * width + x].r = red;
	framebuffer[y * width + x].g = green;
	framebuffer[y * width + x].b = blue;
}

void FrameBuffer::set_depth(int x, int y, float depth) {
	exit_if_out_of_bounds(x, y);

	framebuffer[y * width + x].d = depth;
}

void FrameBuffer::get_fb_min_max_colour(float &minimum, float &maximum) {

	float min = 100.0f; float max = 0.0f;

	for (int i = 0; i < width * height; i++) {
		if (framebuffer[i].r > max) max = framebuffer[i].r;
		if (framebuffer[i].g > max) max = framebuffer[i].g;
		if (framebuffer[i].b > max) max = framebuffer[i].b;

		if (framebuffer[i].r < min) min = framebuffer[i].r;
		if (framebuffer[i].g < min) min = framebuffer[i].g;
		if (framebuffer[i].b < min) min = framebuffer[i].b;
	}

	minimum = min; maximum = max;
}

void FrameBuffer::get_fb_min_max_depth(float &minimum, float &maximum) {

	float min = 100.0f; float max = 0.0f;

	for (int i = 0; i < width * height; i++) {
		if (framebuffer[i].d > max) max = framebuffer[i].d;
		if (framebuffer[i].d < min) min = framebuffer[i].d;
	}

	minimum = min; maximum = max;
}

void FrameBuffer::export_rgb(char *filename) {

	// Calculate buffer values scaling required for writing
	float min, max;
	get_fb_min_max_colour(min, max);

	cerr 	<< endl 
			<< left << setw(20) << "Colour Info" 
			<< left << setw(15) << "Max Exposure" 
			<< left << setw(15) << "Min Exposure" 
			<< left << setw(15) << "Difference"
			<< endl;

	float diff = max - min;
	cerr 	<< left << setw(20) << "Raw"
			<< left << setw(15) << max
			<< left << setw(15) << min
			<< left << setw(15) << diff 
			<< endl;

	// Write to file.
	ofstream outfile;
	outfile.open(filename, ofstream::binary);

	if (!outfile.is_open()) {
		std::ostringstream err;
		err << "ERR: Could not open file \"" << filename << "\" for writing." << endl;
		std::logic_error(err.str());
	}

	try {

		// PPM P6 Header
		outfile << "P6\n";
		outfile << width << " " << height << "\n255\n";

		post_process( 
			CONFIG::EXPOSURE, 
			CONFIG::CONTRAST, 
			CONFIG::BRIGHTNESS, 
			Colour(CONFIG::COLOUR_FILTER_R, CONFIG::COLOUR_FILTER_G, CONFIG::COLOUR_FILTER_B),
			CONFIG::SATURATION, 
			CONFIG::GAMMA
		);

		get_fb_min_max_colour(min, max);

		float diff = max - min;
		cerr 	<< left << setw(20) << "Post"
				<< left << setw(15) << max
				<< left << setw(15) << min
				<< left << setw(15) << diff 
				<< endl << endl;

		for (int i = 0; i < width * height; i++) {
			outfile << (unsigned char)(((framebuffer[i].r)) * 255.0)
					<< (unsigned char)(((framebuffer[i].g)) * 255.0)
					<< (unsigned char)(((framebuffer[i].b)) * 255.0);
		}

		outfile.close();

	} catch (exception e) {
		std::ostringstream err;
		err << "ERR: Unexpected file write error. Filename: \"" << filename << "\". (Did the file close properly?)" << endl;
		std::logic_error(err.str());
	}
}

void FrameBuffer::export_depth(char *filename) {

	// Calculate buffer values scaling required for writing
	float min, max;
	get_fb_min_max_depth(min, max);

	cerr 	<< left << setw(20) << "Depth Info" 
			<< left << setw(15) << "Max Depth" 
			<< left << setw(15) << "Min Depth" 
			<< left << setw(15) << "Difference"
			<< endl;

	float diff = max - min;
	cerr 	<< left << setw(20) << "Raw"
			<< left << setw(15) << max
			<< left << setw(15) << min
			<< left << setw(15) << diff 
			<< endl << endl;

	// Write to file.
	ofstream outfile;
	outfile.open(filename, ofstream::binary);

	if (!outfile.is_open()) {
		std::ostringstream err;
		err << "ERR: Could not open file \"" << filename << "\" for writing." << endl;
		std::logic_error(err.str());
	}

	try {

		// PPM P6 Header
		outfile << "P6\n";
		outfile << width << " " << height << "\n255\n";

		for (int i = 0; i < width * height; i++) {
			unsigned char depth_to_write = (unsigned char)(((framebuffer[i].d - min) / diff) * 255.0);
			outfile << depth_to_write << depth_to_write << depth_to_write; // Write as mono (r=b=g).
		}

		outfile.close();

	} catch (exception e) {
		std::ostringstream err;
		err << "ERR: Unexpected file write error. Filename: \"" << filename << "\". (Did the file close properly?)" << endl;
		std::logic_error(err.str());
	}


}
