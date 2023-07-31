#include <rendering/camera.h>
#include <config/config.h>
#include <util/util.h>

void Camera::look_at(Vector target) {
    target.y = -target.y; // Negate y to make input +ve y's mean up.
    Vector forward = target - position;
    forward.normalise();

    // Arbitrary vector used for orthonormal basis. Note biased such that default up is +ve y.
    Vector temp = Vector(0.0f, 1.0f, 0.0f);
    temp.normalise();

    Vector right = Vector::cross_product(temp, forward);
    Vector up = Vector::cross_product(forward, right);

    Transform camera_to_world = Transform(
        right.x,    up.x,   forward.x,  -position.x,
        right.y,    up.y,   forward.y,  -position.y,
        right.z,    up.z,   forward.z,  -position.z,
        0,          0,      0,          1
    );

    camera_to_world_mat = camera_to_world;

}

/// @brief Look at with up and right specified. Up and right are projected 
///           onto most sensible valid orthonormal basis.
void Camera::look_at(Vector target, Vector up, Vector right) {
    target.y = -target.y; // Negate y to make input +ve y's mean up.
    Vector forward = target - position;
    forward.normalise();

    // Arbitrary vector used for orthonormal basis.
    Vector temp = Vector(0.0f, 1.0f, 0.0f);
    temp.normalise();

    // Calculate up first as up is usually specified over right.
    Vector biased_up = Vector::norm(up - up.dot(forward) * forward); // Project given up onto up-right plane.

    // There are only two possible configurations of right vector from here on out, so just figure out the projection sign.
    Vector unbiased_right = Vector::cross_product(forward, biased_up);
    Vector biased_right = Vector::norm(unbiased_right * right.dot(unbiased_right));

    Transform camera_to_world = Transform(
        biased_right.x,    biased_up.x,   forward.x,  -position.x,
        biased_right.y,    biased_up.y,   forward.y,  -position.y,
        biased_right.z,    biased_up.z,   forward.z,  -position.z,
        0,          0,      0,          1
    );

    camera_to_world_mat = camera_to_world;
}

Ray Camera::get_ray(int x, int y) {

    if (needs_memo) {
        
        params.aspect_ratio = width / height;

        params.fov_tangent = tan(fov / 2.0f * M_PI / 180.0f);
        params.projection_offset = -focal_length;

        Vector world_zero = Vector::zeroes();
        get_cam_to_world_matrix().apply_homogeneous(world_zero, params.ray_origin);

        needs_memo = false;
    }

	Ray camera_ray = Ray();
	camera_ray.position = params.ray_origin;

    float u, v;
    if (CONFIG::SSAA_SAMPLES > 1) {
        // Generate a pixel position with a random offset to the pixel center in interval [-0.5, 0.5)
        u = x + UTIL::rand_float(-0.5f, 0.5f) + 0.5f;
        v = y + UTIL::rand_float(-0.5f, 0.5f) + 0.5f;
    } else {
        // If no supersampling just use pixel center.
        u = x + 0.5f;
        v = y + 0.5f;
    }

    // Pixel as proportion of image resolution
    float pixel_center_x = u / width;
    float pixel_center_y = v / height;
    
    // Stretch to span [-1, -1] -> [1, 1]
    float screen_space_x = (2 * pixel_center_x - 1) * params.aspect_ratio;
    float screen_space_y = 1 - 2 * pixel_center_y;

    // Account for camera FOV
    float camera_x = screen_space_x * params.fov_tangent;
    float camera_y = screen_space_y * params.fov_tangent;

    Vector ray_pixel_point, ray_pixel_point_world;
    ray_pixel_point = Vector(camera_x, camera_y, params.projection_offset);

    // Transform ray intersection with pixel projection point from camera coordinates to world coordinates.
    get_cam_to_world_matrix().apply_homogeneous(ray_pixel_point, ray_pixel_point_world);

    camera_ray.direction = Vector::norm(ray_pixel_point_world - params.ray_origin);

    return camera_ray;
}
