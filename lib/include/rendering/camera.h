#pragma once

#include "../data/object.h"
#include "../data/vector.h"
#include "../data/ray.h"
#include "../data/transform.h"
#include "../data/framebuffer.h"

class Camera : public Object {
private:

    Transform camera_to_world_mat;

public:

    struct memoized_cast_params {
        float aspect_ratio;
        float fov_tangent;
        float projection_offset;
        Vector ray_origin;
    };

    int width;
    int height;
    float fov;
    float focal_length;
    FrameBuffer* fb; 

    memoized_cast_params params;
    bool needs_memo = true;

    Camera(int w, int h) : Camera(
        w, h, 90.f, 1.0f, Vector(0.0f, 0.0f, 0.0f)
    ) {}

    Camera(int w, int h, float f_o_v, float focal_len, Vector pos) {
        width = w;
        height = h;
        fb = new FrameBuffer(w, h);
        fov = f_o_v;
        focal_length = focal_len;
        position = Vector(pos.x, -pos.y, pos.z); // Invert y to conform with convention that +y is up and -y is down.
    }

    void look_at(Vector target);
    void look_at(Vector target, Vector up, Vector right);

    Ray get_ray(int x, int y);

    Transform get_cam_to_world_matrix() {
        return camera_to_world_mat;
    }

};