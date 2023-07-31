#include <rendering/scene.h>
#include <math.h>
#include <chrono>

#include <config/config.h>

#include <util/util.h>

void Scene::render() {

	if (use_photon_map) photon_map();
	else ray_trace();

}

void Scene::photon_map() {

	build_global_photon_map();

	build_caustic_photon_map();

	// Rendering Pass
	render_photon_map(false);
	
}

void Scene::build_global_photon_map() {

	// TODO: Replace with true random russian roulette?
	int total_intensity = 0;

	for (Light *current_light : lights) {
		total_intensity += current_light->intensity;
	} 
	
	// Generate Photon Map
	cerr << "   Generating Global Photon Map" << endl;
	auto t0 = chrono::high_resolution_clock::now();
	int photon_share = CONFIG::NUM_GLOBAL_PHOTONS / total_intensity;
	int n_photons;
	Colour power;

	global_photon_map = KdTree();
	
	int photon_count = 0;
	int udpate_progress_interval = 1000;

	for (Light *current_light : lights) { 

		n_photons = photon_share * current_light->intensity;

		for (int i = 0; i < n_photons; i++) {
			Ray photon_ray = Ray();
			current_light->generate_photon(photon_ray.position, photon_ray.direction);

			trace_global_photon(photon_ray, current_light->get_emission(), CONFIG::MAX_RECURSION_DEPTH);
			photon_count++;

			if (photon_count % udpate_progress_interval == 0) {
				UTIL::display_progress(CONFIG::NUM_GLOBAL_PHOTONS, photon_count, (char *)"photons", (char *)"");
			}
		}
	}

	UTIL::display_progress(CONFIG::NUM_GLOBAL_PHOTONS, CONFIG::NUM_GLOBAL_PHOTONS, (char *)"photons", (char *)" baking...");

	global_photon_map.bake_tree();

	auto t1 = chrono::high_resolution_clock::now();
	int seconds, minutes, hours;
	UTIL::chrono_total_time_split(
		chrono::duration_cast<chrono::seconds>(t1 - t0).count(),
		seconds, minutes, hours
	);

	std::ostringstream done_str;
	done_str 	<< "done [" << hours << "h " << minutes << "m " << seconds << "s] " 
				<< "(stored " << global_photon_map.get_size() << ")";

	UTIL::display_progress(
		CONFIG::NUM_GLOBAL_PHOTONS, CONFIG::NUM_GLOBAL_PHOTONS, (char *)"photons", (char *)done_str.str().c_str()
	);
	cerr << endl << endl;
}

void Scene::trace_global_photon(Ray photon_ray, Colour photon_pow, int recursion_depth) {

	if (recursion_depth < 0) return;

	Hit hit;

	if (intersection_trace(photon_ray, hit, true)) {

		// Russian Roulette
		float epsilon = UTIL::rand_float(0.00000001f, 1.0f);
		
		Colour obj_d_albedo = hit.obj->material->albedo->map_uv(hit.u, hit.v);
		Colour obj_s_albedo = hit.obj->material->specular_albedo->map_uv(hit.u, hit.v);
		float d = (obj_d_albedo.r + obj_d_albedo.g + obj_d_albedo.b) / 3.0f;
		float s = (obj_s_albedo.r + obj_s_albedo.g + obj_s_albedo.b) / 3.0f;
		
		// Diffuse Reflection
		if (epsilon <= d) {

			Ray new_ray = Ray();
			new_ray.position = hit.position;

			float pdf;
			Colour fr = hit.obj->material->sample_brdf(
				hit, 
				-photon_ray.direction, new_ray.direction, 
				pdf);

			UTIL::small_offset_ray(new_ray);

			if (recursion_depth < CONFIG::MAX_RECURSION_DEPTH) // Indirect Illum only.
				global_photon_map.add(Photon(hit.position, -photon_ray.direction, photon_pow));
			
			photon_pow *= obj_d_albedo / d;

			trace_global_photon(new_ray, photon_pow, recursion_depth - 1);
		}
		// Specular Reflection
		else if (epsilon <= s + d) {
			
			// Reflect photon - This should really be part of the material to allow fuzzy reflection etc,
			//    but for now simple glossy reflection is fine.

			Ray new_ray = Ray(
				hit.position,
				Vector::reflect(photon_ray.direction, hit.normal)
			);

			UTIL::small_offset_ray(new_ray);

			photon_pow /= s;

			trace_global_photon(new_ray, photon_pow, recursion_depth - 1);

		} 
		// Absorption - return not needed but this clause included for clarity.
		else {
			return;
		}
	}
}

void Scene::build_caustic_photon_map() {

	// TODO: Replace with true random russian roulette?
	int total_intensity = 0;

	for (Light *current_light : lights) {
		total_intensity += current_light->intensity;
	} 
	
	// Generate Photon Map
	cerr << "   Generating Caustic Photon Map" << endl;
	auto t0 = chrono::high_resolution_clock::now();
	int photon_share = CONFIG::NUM_CAUSTIC_PHOTONS / total_intensity;
	int n_photons;
	Colour power;

	caustic_photon_map = KdTree();
	
	int photon_count = 0;
	int udpate_progress_interval = 1000;

	for (Light *current_light : lights) { 

		n_photons = photon_share * current_light->intensity;

		for (int i = 0; i < n_photons; i++) {
			Ray photon_ray = Ray();
			current_light->generate_photon(photon_ray.position, photon_ray.direction);

			trace_caustic_photon(photon_ray, current_light->get_emission(), false, CONFIG::MAX_RECURSION_DEPTH);
			photon_count++;

			if (photon_count % udpate_progress_interval == 0) {
				UTIL::display_progress(CONFIG::NUM_CAUSTIC_PHOTONS, photon_count, (char *)"photons", (char *)"");
			}
		}
	}

	UTIL::display_progress(CONFIG::NUM_CAUSTIC_PHOTONS, CONFIG::NUM_CAUSTIC_PHOTONS, (char *)"photons", (char *)" baking...");

	caustic_photon_map.bake_tree();

	auto t1 = chrono::high_resolution_clock::now();
	int seconds, minutes, hours;
	UTIL::chrono_total_time_split(
		chrono::duration_cast<chrono::seconds>(t1 - t0).count(),
		seconds, minutes, hours
	);

	std::ostringstream done_str;
	done_str 	<< "done [" << hours << "h " << minutes << "m " << seconds << "s] " 
				<< "(stored " << caustic_photon_map.get_size() << ")";

	UTIL::display_progress(
		CONFIG::NUM_CAUSTIC_PHOTONS, CONFIG::NUM_CAUSTIC_PHOTONS, (char *)"photons", (char *)done_str.str().c_str()
	);
	cerr << endl << endl;

}

void Scene::trace_caustic_photon(Ray photon_ray, Colour photon_pow, bool hit_specular, int recursion_depth) {

	if (recursion_depth < 0) return;

	Hit hit;
	if (intersection_trace(photon_ray, hit, true)) {
		
		// TODO: Consider Emissive Material hit?

		if (hit.obj->material->mat_type == MATERIAL_TYPE::DIFFUSE) {
			if (!hit_specular) return; // Don't store photons unless they have been specularly transmitted at least once.
			caustic_photon_map.add(Photon(hit.position, -photon_ray.direction, photon_pow));
			return;
		}

		// For next pass, we must only store photons if this trace was a specular reflection.
		hit_specular = hit.obj->material->mat_type == MATERIAL_TYPE::SPECULAR;

		// Russian Roulette
		float epsilon = UTIL::rand_float(0.0f, 1.0f);
		
		Colour specular_albedo = hit.obj->material->specular_albedo->map_uv(hit.u, hit.v);
		float s = (specular_albedo.r + specular_albedo.g + specular_albedo.b) / 3.0f;
		
		// Specular Reflection
		if (epsilon <= s) {

			Ray new_ray = Ray();
			new_ray.position = hit.position;

			float pdf;
			Colour fr = hit.obj->material->sample_brdf(
				hit, 
				photon_ray.direction, new_ray.direction, 
				pdf);

			UTIL::small_offset_ray(new_ray);

			photon_pow *= specular_albedo / s;

			// TODO, could allow refraction to not decrease depth here, though probably (definitely) unnecessary for small scenes.
			trace_caustic_photon(new_ray, photon_pow, hit_specular, recursion_depth - 1);
		}
		// Absorption - return not needed but this clause included for clarity.
		else {
			return;
		}
	}
}

void Scene::vizualise_photon_map(Ray ray, Colour &colour) {
	
	Hit hit;

	if (intersection_trace(ray, hit)) {
		float radius_squared;
		vector<Photon> close = caustic_photon_map.get_estimate_photons(hit.position, 1, radius_squared);

		Vector wi;
		float scatter_pdf;
		Colour fr = hit.obj->material->sample_brdf(hit, -ray.direction, wi, scatter_pdf);

		const Photon &p = close.front();
		if (radius_squared < 0.0005f) {
			// colour = fr * std::abs(wi.dot(hit.normal)) * p.flux / scatter_pdf;
			colour = 1.0f / 10.0f * p.flux;
		}
	}
}

void Scene::render_photon_map(const bool vizualise) {
	UTIL::display_render_progress(camera->width, camera->height, 0, 0);

	for (int y = 0; y < camera->height; y++) {
		for (int x = 0; x < camera->width; x++) {

			Colour pixel_colour;
			float pixel_depth;

			// Supersampling
			for (int s = 0; s < CONFIG::SSAA_SAMPLES; s++) {
				Ray camera_ray = camera->get_ray(x, y);

				//Raytrace each supersampled ray and aggregate results
				Colour Lr;
				float depth = 0;

				if (vizualise) {
					vizualise_photon_map(camera_ray, Lr);
				} else {
					// Lr = preview_specular_and_glossy(camera_ray, CONFIG::MAX_RECURSION_DEPTH);
					Lr = render_photon_map_recursive(camera_ray, CONFIG::MAX_RECURSION_DEPTH);
				}

				pixel_colour += Lr;
				pixel_depth += depth;

			}

			// Average the samples.
			pixel_colour /= CONFIG::SSAA_SAMPLES;
			pixel_depth /= CONFIG::SSAA_SAMPLES;
		
			// Output to framebuffer.
			camera->fb->set_colour(x, y, pixel_colour.r, pixel_colour.g, pixel_colour.b);
			camera->fb->set_depth(x, y, pixel_depth);

		}

		UTIL::display_render_progress(camera->width, camera->height, camera->width, y);

	}
}

Colour Scene::preview_specular_and_glossy(Ray ray, int recursion_depth) {
	
	if (recursion_depth < 0) return Colour(0.0f);
	
	// Calculate the rendering equation Lr = direct illum. + specular +  caustics + indirect illum.
	//   Denoted respectively by Li_l + (Li_c + Li_d) + Li_c + Li_d.
	Hit hit;
	
	if (intersection_trace(ray, hit)) {
		Colour Li_l, Li_cd, Li_c, Li_d; // Don't consider specular when hitting diffuse surfaces.
		float c_l = 1.0f;
		float c_cd = 1.0f;
		float c_c = 0.025f;
		float c_d = 0.25f;

		Vector wr = -ray.direction;

		Li_cd = get_specular_and_glossy(-wr, hit, recursion_depth);

		return Li_cd * c_cd;
	}
	
	return void_colour;
}

Colour Scene::render_photon_map_recursive(Ray ray, int recursion_depth) {

	if (recursion_depth < 0) return Colour(0.0f);
	
	// Calculate the rendering equation Lr = direct illum. + specular +  caustics + indirect illum.
	//   Denoted respectively by Li_l + (Li_c + Li_d) + Li_c + Li_d.
	Hit hit;
	
	if (intersection_trace(ray, hit)) {
		Colour Li_l, Li_cd, Li_c, Li_d; // Don't consider specular when hitting diffuse surfaces.
		float c_l = 1.0f;
		float c_cd = 1.0f;
		float c_c = 0.0175f;
		float c_d = 0.25f;

		Vector wr = -ray.direction;

		Li_cd = get_specular_and_glossy(-wr, hit, recursion_depth);

		Li_l = get_direct_illumination(wr, hit);

		if (hit.obj->material->mat_type == MATERIAL_TYPE::DIFFUSE) {

			Li_c = compute_caustic_radiance(wr, hit);

			Li_d = compute_global_radiance(wr, hit);
		}

		return c_l * Li_l + c_cd * Li_cd + c_c * Li_c +  c_d * Li_d;
	}
	
	return void_colour;
}

Colour Scene::get_direct_illumination(Vector wr, Hit hit) {
	Colour Lr = Colour(0.0f);
	Colour Le = hit.obj->material->evaluate_emissive(hit);

	for (Light *current_light : lights) {

		if (hit.obj->material->mat_type == MATERIAL_TYPE::DIFFUSE) {
			Vector wi;
			// Early exit if light is facing opposite direction.
			if(!current_light->get_direction(hit.position, hit.normal, wi)) {Lr += Le; continue;}


			float n_shadow_samples = CONFIG::PENUMBRA_SAMPLES;
			if (current_light->constant_sample) n_shadow_samples = 1.0f;

			float shadow_coeff = 0.0f;
			float _;
			// Distribution Raycasting for area lights.
			for (int i = 0; i < n_shadow_samples; i++) {
				// Check for shadows via raycast.
				Vector sample_pos = n_shadow_samples == 1.0f ? current_light->position : current_light->sample_position(_);
				Vector shadow_cast_dir = Vector::norm(sample_pos - hit.position);
				Ray shadow_ray(hit.position, shadow_cast_dir);
				UTIL::small_offset_ray(shadow_ray);
				Hit shadow_hit;

				bool shadow_flag = intersection_trace(shadow_ray, shadow_hit);

				// Do not cast shadows from emissive materials.
				if (shadow_flag && shadow_hit.obj->material->mat_type == MATERIAL_TYPE::EMISSIVE) shadow_flag = false;

				// TODO: Length-squared comparison may be a useful optimisation here.
				float far_t = (shadow_ray.position - sample_pos).length() / shadow_ray.direction.length();
				if(shadow_flag && shadow_hit.t < far_t) shadow_coeff++;
			}
			shadow_coeff /= n_shadow_samples; // Proportion of visible samples.

			float Li_coeff = 1.0f - shadow_coeff;
			if (Li_coeff == 0.0f) {Lr += Le; continue;}

			// If no shadows, evaluate rendering eq: f(x, wi, wr) * Li * wi . n
			//   Note in whitted raytracing, the integral is 'approximated' by taking a sample to each visible light.
			
			Colour Li = current_light->get_intensity(hit.position); // Incoming Radiance
			Colour fr = hit.obj->material->evaluate_brdf(hit, wi, -wr); // BRDF.
			
			Lr += Le + fr * Li * std::fabs(wi.dot(hit.normal)) * Li_coeff;
		}

		if (hit.obj->material->mat_type == MATERIAL_TYPE::EMISSIVE) Lr += Le;

	}

	return Lr;
}

Colour Scene::get_specular_and_glossy(Vector wr, Hit hit, int recursion_depth) {
		
		Colour Lr = Colour(0.0f);
		Colour Le = hit.obj->material->evaluate_emissive(hit);

		for (Light *current_light : lights) {

			// Specular materials not affected by shadow directly.
			if (hit.obj->material->mat_type == MATERIAL_TYPE::SPECULAR) {
				// For specular materials, we recursively sample transmitted / reflected light.
				//   The material determines how the samples are mixed, i.e. their individual subsequent brdf evaluations f(x, wi, wr).
				
				Colour Lr_s = Colour(0.0f);
				auto specular_info = hit.obj->material->multi_sample_brdf(hit, wr);
				
				for (auto &s_info : specular_info) {

					Vector specular_wi = std::get<0>(s_info);
					Colour specular_fr = std::get<1>(s_info);
					TRANSMISSION_TYPE specular_tt = std::get<2>(s_info);

					Ray specular_ray(hit.position, specular_wi);

					int delta_depth;

					// Handle offset edge cases for transmission types slightly differently
					if (specular_tt == TRANSMISSION_TYPE::REFLECTION) {
						delta_depth = -1;
						UTIL::small_offset_ray(specular_ray); // For reflection we can offset just based on the direction of the ray.
					} else {
						delta_depth = 0;
						Vector ray_bias = wr.dot(hit.normal) < 0 ? -hit.normal : hit.normal;
						UTIL::small_offset_ray(specular_ray, ray_bias); // For refraction, offset via the normal as oblique cases may not offset the ray enough.
					}

					float _; // Throw away depth as only consider depth for first obj hit.

					// Recursive rendering eq. f(x, wi, wr) * Li * wi . n
					//   where Li is calculated from casting a specular ray.
					Lr_s += specular_fr * render_photon_map_recursive(specular_ray, recursion_depth + delta_depth);
				}

				Lr += Lr_s;
			}
		}

		return Lr;
}

Colour Scene::compute_caustic_radiance(Vector wr, Hit hit) {

	float radius_squared;
	vector<Photon> close = caustic_photon_map.get_estimate_photons(
		hit.position, CONFIG::PHOTONS_PER_ESTIMATE, radius_squared
	);

	// float radius_squared = 0.2f;
	// vector<Photon> close = caustic_photon_map.get_estimate_photons_bounded(
	// 	hit.position, CONFIG::PHOTONS_PER_ESTIMATE, radius_squared);
	
	Colour fr;
	Colour radiance;
	for (Photon p : close) {
		if (p.wi.dot(hit.normal) < 0) continue;

		fr = hit.obj->material->evaluate_brdf(hit, p.wi, wr);
		radiance += fr * p.flux * UTIL::gaussian_filter((p.pos - hit.position).length_squared(), radius_squared);
	}

	return 2500 * radiance / (M_PI * radius_squared * CONFIG::NUM_GLOBAL_PHOTONS);
}

Colour Scene::compute_global_radiance(Vector wr, Hit hit) {

	float radius_squared = 0.2f;
	vector<Photon> close = global_photon_map.get_estimate_photons_bounded(
		hit.position, CONFIG::PHOTONS_PER_ESTIMATE, radius_squared);
	
	Colour fr;
	Colour radiance;
	for (Photon p : close) {
		if (p.wi.dot(hit.normal) < 0) continue;

		fr = hit.obj->material->evaluate_brdf(hit, p.wi, wr);
		radiance += fr * p.flux;
	}

	return 350 * radiance / (M_PI * radius_squared * CONFIG::NUM_GLOBAL_PHOTONS);
}

void Scene::ray_trace() {
	UTIL::display_render_progress(camera->width, camera->height, 0, 0);

	for (int y = 0; y < camera->height; y++) {
		for (int x = 0; x < camera->width; x++) {

			Colour pixel_colour;
			float pixel_depth = 0;

			// Supersampling
			for (int s = 0; s < CONFIG::SSAA_SAMPLES; s++) {
				Ray camera_ray = camera->get_ray(x, y);

				// Raytrace each supersampled ray and aggregate results
				float depth = 0;
				Colour Lr = cast_ray(camera_ray, CONFIG::MAX_RECURSION_DEPTH, depth); // Reflected Radiance for pixel.

				pixel_colour += Lr;
				pixel_depth += depth;

			}

			// Average the samples.
			pixel_colour /= CONFIG::SSAA_SAMPLES;
			pixel_depth /= CONFIG::SSAA_SAMPLES;
		
			// Output to framebuffer.
			camera->fb->set_colour(x, y, pixel_colour.r, pixel_colour.g, pixel_colour.b);
			camera->fb->set_depth(x, y, pixel_depth);

		}

		UTIL::display_render_progress(camera->width, camera->height, camera->width, y);
	}
}

bool Scene::intersection_trace(Ray ray, Hit &closest_hit, bool ignore_emissive) {

	bool closest_hit_flag = false;
	bool hit_flag;

	for (Object3D *current_object : objects) {

		if (
			ignore_emissive &&
			current_object->material && 
			current_object->material->mat_type == MATERIAL_TYPE::EMISSIVE
		) continue;

		Hit obj_hit;
		hit_flag = current_object->get_closest_intersection(ray, obj_hit);
	
		if (hit_flag) {
			// Ensure intersection not behind camera.
	  		if (obj_hit.t > 0.0f) {
				if (!closest_hit_flag || (closest_hit_flag && obj_hit.t < closest_hit.t)) {
					closest_hit = obj_hit;
					closest_hit_flag = true;
				}
	  		}
		}
	}

	return closest_hit_flag;
}

Colour Scene::cast_ray(Ray ray, int recursion_depth, float &depth) {

	if (recursion_depth < 0) return Colour(0.0f);

	Hit hit;
	if (intersection_trace(ray, hit)) {

		depth = hit.t;
		
		Colour Lr = Colour(0.0f);
		Colour Le = hit.obj->material->evaluate_emissive(hit);

		for (Light *current_light : lights) {

			Vector wr = ray.direction;

			if (hit.obj->material->mat_type == MATERIAL_TYPE::DIFFUSE) {
				Vector wi;
				// Early exit if light is facing opposite direction.
				if(!current_light->get_direction(hit.position, hit.normal, wi)) {Lr += Le; continue;}


				float n_shadow_samples = CONFIG::PENUMBRA_SAMPLES;
				if (current_light->constant_sample) n_shadow_samples = 1.0f;

				float shadow_coeff = 0.0f;
				float _;
				// Distribution Raycasting for area lights.
				for (int i = 0; i < n_shadow_samples; i++) {
					// Check for shadows via raycast.
					Vector sample_pos = n_shadow_samples == 1.0f ? current_light->position : current_light->sample_position(_);
					Vector shadow_cast_dir = Vector::norm(sample_pos - hit.position);
					Ray shadow_ray(hit.position, shadow_cast_dir);
					UTIL::small_offset_ray(shadow_ray);
					Hit shadow_hit;

					bool shadow_flag = intersection_trace(shadow_ray, shadow_hit);

					// Do not cast shadows from emissive materials.
					if (shadow_flag && shadow_hit.obj->material->mat_type == MATERIAL_TYPE::EMISSIVE) shadow_flag = false;

					// TODO: Length-squared comparison may be a useful optimisation here.
					float far_t = (shadow_ray.position - sample_pos).length() / shadow_ray.direction.length();
					if(shadow_flag && shadow_hit.t < far_t) shadow_coeff++;
				}
				shadow_coeff /= n_shadow_samples; // Proportion of visible samples.

				float Li_coeff = 1.0f - shadow_coeff;
				if (Li_coeff == 0.0f) {Lr += Le; continue;}

				// If no shadows, evaluate rendering eq: f(x, wi, wr) * Li * wi . n
				//   Note in whitted raytracing, the integral is 'approximated' by taking a sample to each visible light.
				
				Colour Li = current_light->get_intensity(hit.position); // Incoming Radiance
				Colour fr = hit.obj->material->evaluate_brdf(hit, wi, -wr); // BRDF.
				
				Lr += Le + fr * Li * std::fabs(wi.dot(hit.normal)) * Li_coeff;
			}

			// Specular materials not affected by shadow directly.
			if (hit.obj->material->mat_type == MATERIAL_TYPE::SPECULAR) {
				// For specular materials, we recursively sample transmitted / reflected light.
				//   The material determines how the samples are mixed, i.e. their individual subsequent brdf evaluations f(x, wi, wr).
				
				Colour Lr_s = Colour(0.0f);
				auto specular_info = hit.obj->material->multi_sample_brdf(hit, wr);
				
				for (auto &s_info : specular_info) {

					Vector specular_wi = std::get<0>(s_info);
					Colour specular_fr = std::get<1>(s_info);
					TRANSMISSION_TYPE specular_tt = std::get<2>(s_info);

					Ray specular_ray(hit.position, specular_wi);

					int delta_depth;

					// Handle offset edge cases for transmission types slightly differently
					if (specular_tt == TRANSMISSION_TYPE::REFLECTION) {
						delta_depth = -1;
						UTIL::small_offset_ray(specular_ray); // For reflection we can offset just based on the direction of the ray.
					} else {
						delta_depth = 0;
						Vector ray_bias = wr.dot(hit.normal) < 0 ? -hit.normal : hit.normal;
						UTIL::small_offset_ray(specular_ray, ray_bias); // For refraction, offset via the normal as oblique cases may not offset the ray enough.
					}

					float _; // Throw away depth as only consider depth for first obj hit.

					// Recursive rendering eq. f(x, wi, wr) * Li * wi . n
					//   where Li is calculated from casting a specular ray.
					Lr_s += specular_fr * cast_ray(specular_ray, recursion_depth + delta_depth, _);
				}

				Lr += Lr_s;
			}

			if (hit.obj->material->mat_type == MATERIAL_TYPE::EMISSIVE) Lr += Le;

		}

		return Lr;
	}

	// Ray went to void
	return void_colour;
}