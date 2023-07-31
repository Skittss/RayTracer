#include <geometry/sphere.h>

#include <vector>
#include <math.h>

Sphere::Sphere(Vector c, float r) : center(c), radius(r) {}

void Sphere::get_uv(Vector pos, float &u, float &v) {
	float theta = acos(-pos.y);
	float phi   = atan2(-pos.z, pos.x) + M_PI;

	u = phi / (2 * phi);
	v = theta / M_PI;
}

vector<Hit> Sphere::intersection(Ray ray)
{
	vector<Hit> hits;

	// Note: simplification of quadratic formula by assuming b = some 2n.
	Vector ray_origin_to_center = ray.position - center;
	float a = ray.direction.length_squared();
	float b_over_two = ray_origin_to_center.dot(ray.direction);
	float c = ray_origin_to_center.length_squared() - radius * radius;
	float discriminant = b_over_two * b_over_two - a * c;

	if (discriminant < 0.0f) return hits; // No intersection if discrim < 0

	float sqrt_d = sqrtf(discriminant);

	// Check smallest root first
	float t = (-b_over_two - sqrt_d) / a;
	Hit small_root;
	small_root.obj = this; 						
	small_root.t = (-b_over_two - sqrt_d) / a; 
	small_root.position = ray.position + small_root.t * ray.direction;
	small_root.normal = (small_root.position - center) / radius;
	small_root.entering = true;
	get_uv(small_root.normal, small_root.u, small_root.v);
	hits.emplace_back(small_root);

	// Check larger root
	t =  (-b_over_two + sqrt_d) / a;
	Hit large_root;
	large_root.obj = this;
	large_root.t = (-b_over_two + sqrt_d) / a;
	large_root.position = ray.position + large_root.t * ray.direction;
	large_root.normal = (large_root.position - center) / radius;
	large_root.entering = false;
	get_uv(large_root.normal, large_root.u, large_root.v);
	hits.emplace_back(large_root);

	return hits;

}
