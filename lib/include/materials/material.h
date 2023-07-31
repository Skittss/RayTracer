#pragma once

#include <vector>
#include <tuple>

#include "../data/vector.h"
#include "../data/colour.h"
#include "../data/hit.h"
#include "../textures/texture.h"

enum class MATERIAL_TYPE { DIFFUSE, SPECULAR, EMISSIVE };
enum class TRANSMISSION_TYPE { REFLECTION, REFRACTION };

class Material {
public:

	MATERIAL_TYPE mat_type = MATERIAL_TYPE::DIFFUSE;

	Texture *albedo; // Default diffuse texture.
	Texture *specular_albedo; // Reflection texture - Used mostly in photon mapping, but also in glossy and specular materials.

	Material() : Material(MATERIAL_TYPE::DIFFUSE) {}
	Material(MATERIAL_TYPE type) : mat_type(type) {}

	/// @brief Emissive colour of object. Also doubles as ambient for phong model.
	virtual Colour evaluate_emissive(const Hit &hit) {
		return Colour(0.0f);
	}

	/// @brief Evaluate the brdf for a given wi and wr.
	/// @param hit Intersection info (mostly for providing x, n for the brdf).
	/// @param wi Incident direction.
	/// @param wr Reflection direction.
	/// @return Evaluation of the BRDF, i.e. f(x, wi, wr)
	virtual Colour evaluate_brdf(const Hit &hit, const Vector &wi, const Vector &wr) {
		return Colour(0.0f);
	}
	
	/// @brief Sample a reflectance direction from the brdf.
	/// @param hit Intersection info (mostly for providing x, n for the brdf).
	/// @param wi Incident direction.
	/// @param wr Reflection direction (Calculated from this function).
	/// @param prob Importance sampled PDF for wr.
	/// @return Evaluation of the BRDF, i.e. f(x, wi, wr)
	virtual Colour sample_brdf(const Hit &hit, const Vector &wi, Vector &wr, float &prob) {
		return evaluate_brdf(hit, wi, wr);
	}

	/// @brief Sample multiple reflectance directions from the brdf. This is used for complex materials where
	///             multiple samples are needed; like dielectrics where both reflection and transmission occurs.
	/// @param hit Intersection info (mostly for providing x, n for the brdf).
	/// @param wi Incident direction
	/// @return List of <Vector, Colour> Pairs which correspond to <wr, f(x, wr, wi)> for each sample.
	virtual vector<std::tuple<Vector, Colour, TRANSMISSION_TYPE>> multi_sample_brdf(const Hit &hit, Vector wi) {
		vector<std::tuple<Vector, Colour, TRANSMISSION_TYPE>> pairs(1);
		Vector wr;
		float _;
		Colour fr = sample_brdf(hit, wi, wr, _);
		pairs[0] = std::tuple<Vector, Colour, TRANSMISSION_TYPE>(wr, fr, TRANSMISSION_TYPE::REFLECTION);
		return pairs;
	}

};
