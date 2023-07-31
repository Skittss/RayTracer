#pragma once

#include <stdlib.h>

#include "../data/vector.h"
#include "../data/colour.h"

class Light {
public:

	Colour colour;
	float intensity;
	Vector position;
	bool constant_sample = true;
	
	Light() : intensity(1.0f), colour(Colour(1.0f)), position(Vector(0.0f)) {}

	Colour get_emission() {
		return colour * intensity;
	}

	/// @brief Get direction from surface to the light
	/// @param surface_pt surface hit point
	/// @param surface_norm surface normal
	/// @param out_dir direction to light from surface
	/// @return true if surface faces the light
	virtual bool get_direction(const Vector &surface_pt, const Vector &surface_norm, Vector &out_dir) = 0;

	// Intensity of the light at the surface.
	virtual Colour get_intensity(const Vector &surface_pt) = 0;

	virtual Vector sample_position(float &pdf) {
		return position;
	}

	virtual float dot_with_normal(Vector o) = 0; // Used for determining the directional pdf of emitted photons.

	virtual void generate_photon(Vector &photon_origin, Vector &photon_dir) {}

};
