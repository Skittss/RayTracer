#include "../data/vector.h"
#include "../data/ray.h"
#include "../data/colour.h"

namespace UTIL {

void display_progress(int total, int i, char *counter, char *label);

void display_render_progress(int dim_x, int dim_y, int x, int y);

void chrono_total_time_split(const int &total, int &s, int &m, int &h);

float clamp(float n, float min, float max);

float lerp(float a, float b, float r);

float rand_float(float min, float max);

float gaussian_filter(float d, float r);

void small_offset_ray(Ray &ray);
void small_offset_ray(Ray &ray, Vector bias);


void get_orthonormal_bases(Vector normal, Vector &tangent, Vector &bitangent);
void get_orthonormal_bases(Vector normal, Vector right, Vector &tangent, Vector &bitangent);

// Shorthand matrix transformations using orthonormal basis of vector to transform about.
Vector world_to_local(Vector v, Vector local_ortho_x, Vector local_ortho_y, Vector local_ortho_z);
Vector local_to_world(Vector v, Vector local_ortho_x, Vector local_ortho_y, Vector local_ortho_z);

Vector spherical_to_cartesian(float theta, float phi);

Vector hemisphere_scatter(float &pdf);

}