#pragma once

#include <vector>

#include "../data/object.h"
#include "../data/ray.h"
#include "../data/hit.h"
#include "../materials/material.h"

class Object3D : public Object {
public:

	Material *material;

	Object3D() {}
	
	/// @brief Gets intersection information between a cast ray
	/// and this object's geometry.
	/// @param ray Ray being cast.
	/// @return vector of hits **SORTED BY ASCENDING DISTANCE** i.e. closest = hits[0].
	virtual vector<Hit> intersection(Ray ray) = 0;

	virtual bool get_closest_intersection(Ray ray, Hit &hit) {
		vector<Hit> hits = intersection(ray);
		if (hits.size() == 0) return false; 
		// Find first non-negative t-value hit
		for (Hit h : hits) {
			if (h.t > 0.0f) {
				hit = h;
				return true;
			}
		}
		return false;
	}
};
