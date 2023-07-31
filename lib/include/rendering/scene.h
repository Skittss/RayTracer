#pragma once

#include <functional>

#include "../data/colour.h"
#include "../data/ray.h"
#include "../data/hit.h"
#include "../data/photon.h"
#include "../data/framebuffer.h"
#include "../data/kd_tree.h"

#include "../geometry/object3D.h"
#include "../lights/light.h"

#include "camera.h"

class Scene {
public:

	Camera *camera;
	vector<Object3D*> objects;
	vector<Light*> lights;
	Colour void_colour;
	bool use_photon_map;

	Scene(Camera *scene_camera, vector<Object3D*> scene_objects, vector<Light*> scene_lights, bool use_photon_map) {
		camera = scene_camera;
		objects = scene_objects;
		lights = scene_lights;
		void_colour = Colour(0.0f);
		this->use_photon_map = use_photon_map;
	}

	Scene(
		Camera *scene_camera, vector<Object3D*> scene_objects, vector<Light*> scene_lights, 
		Colour void_colour, bool use_photon_map
	) : Scene(scene_camera, scene_objects, scene_lights, use_photon_map) {
		this->void_colour = void_colour;
	}

	void render();

private:

	KdTree global_photon_map;
	KdTree caustic_photon_map;

	bool intersection_trace(Ray ray, Hit &closest_hit, bool ignore_emissive = false);


	// Direct Illumination Raytracing Funcs
	void ray_trace();
	Colour cast_ray(Ray ray, int recursion_depth, float &depth);


	// Photon Mapping funcs
	void photon_map();

	void build_global_photon_map();
	void trace_global_photon(Ray photon_ray, Colour photon_pow, int recursion_depth);

	void build_caustic_photon_map();
	void trace_caustic_photon(Ray photon_ray, Colour photon_pow, bool hit_specular, int recursion_depth);

	void vizualise_photon_map(Ray ray, Colour &colour);

	void render_photon_map(const bool vizualise);
	Colour render_photon_map_recursive(Ray ray, int recursion_depth);
	Colour preview_specular_and_glossy(Ray ray, int recursion_depth);
	
	Colour get_direct_illumination(Vector wr, Hit hit);

	Colour get_specular_and_glossy(Vector wr, Hit hit, int recursion_depth);
	
	Colour compute_caustic_radiance(Vector wr, Hit hit);

	Colour compute_global_radiance(Vector wr, Hit hit);

};
