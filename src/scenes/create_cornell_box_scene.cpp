#include "create_cornell_box_scene.h"

Scene create_cornell_box_scene() {

    float PLANE_R = 6.0f;
    float PLANE_D = 2.0f * PLANE_R;
    float SEAM_OFFSET = 0.01f;
    Camera *camera = new Camera(
		CONFIG::WIDTH, 
		CONFIG::HEIGHT, 
		90, // Not drawn from config so the whole scene is in view. 
		CONFIG::CAMERA_FOCAL_LENGTH,
		Vector(0.0f, PLANE_R, 1.5f * PLANE_R)
	);
    camera->look_at(Vector(0.0f, PLANE_R, 0.0f));

	// PointLight *light = new PointLight(
	// 	Vector(0.0f, PLANE_D - 2.0f, 0.0f),
	// 	// Colour(1.0f, 0.9f, 0.7f),
    //     Colour(1.0f),
	// 	160.0f,
	// 	4.0f
	// );

    SquareLight *light = new SquareLight(
        Vector(0.0f, PLANE_D - 0.1f, 0.0f),
        Vector(0.0f, -1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		10.0f,
		4.0f,
        16.0f
    );

	// Phong *wall_red = new Phong(
	// 	Colour(0.0f, 0.0f, 0.0f), 1.0f,
	// 	Colour(1.0f, 0.43f, 0.41f), 1.0f,
	// 	Colour(1.0f, 1.0f, 1.0f), 1.0f,
	// 	20.0f
	// );

    Lambertian *wall_red = new Lambertian(
        Colour(1.0f, 0.43f, 0.41f)
    );

    // Phong *wall_green = new Phong(
    //     Colour(0.0f, 0.0f, 0.0f), 1.0f,
	// 	Colour(0.74f, 0.90f, 0.69f), 1.0f,
	// 	Colour(1.0f, 1.0f, 1.0f), 1.0f,
	// 	20.0f
    // );

    Lambertian *wall_green = new Lambertian(
        Colour(0.74f, 0.90f, 0.69f)
    );

    // Phong *wall_white = new Phong(
    //     Colour(0.0f, 0.0f, 0.0f), 1.0f,
	// 	Colour(1.0f, 1.0f, 1.0f), 1.0f,
	// 	Colour(1.0f, 1.0f, 1.0f), 1.0f,
	// 	20.0f
    // );

    Lambertian *wall_white = new Lambertian(
		Colour(1.0f, 1.0f, 1.0f)
    );

    // Phong *wall_checker = new Phong(
	// 	Colour(0.01f, 0.01f, 0.01f), 1.0f,
	// 	Colour(1.0f, 1.0f, 1.0f), 1.0f,
	// 	Colour(1.0f, 0.25f, 0.0f), 1.0f,
	// 	20.0f
	// );

    Lambertian *wall_checker = new Lambertian(
        Colour(1.0f, 1.0f, 1.0f)
    );
	wall_checker->apply_texture(new CheckerTexture(
		Colour(0.1f, 0.1f, 0.1f),
		Colour(0.9f, 0.9f, 0.9f),
		1.0f, 1.0f
	));

    Vector *floor_pos = new Vector(0.0f, 0.0f, 0.0f);
	Vector *floor_up  = new Vector(0.0f, 1.0f, 0.0f);

    Vector *ceil_pos  = new Vector(0.0f, PLANE_D - SEAM_OFFSET, 0.0f);
	Vector *ceil_up   = new Vector(0.0f, -1.0f, 0.0f);

    Vector *back_pos  = new Vector(0.0f, PLANE_R, PLANE_R - SEAM_OFFSET);
    Vector *back_up   = new Vector(0.0f, 0.0f, -1.0f);

    Vector *green_pos = new Vector(-PLANE_R + SEAM_OFFSET, PLANE_R, 0.0f);
    Vector *green_up  = new Vector(1.0f, 0.0f, 0.0f);

    Vector *red_pos   = new Vector(PLANE_R - SEAM_OFFSET, PLANE_R, 0.0f);
    Vector *red_up    = new Vector(-1.0f, 0.0f, 0.0f);

    Plane *floor = new Plane(*floor_pos, PLANE_D, *floor_up);
    Plane *ceil  = new Plane(*ceil_pos,  PLANE_D, *ceil_up );
    Plane *back  = new Plane(*back_pos,  PLANE_D, *back_up );
    Plane *green = new Plane(*green_pos, PLANE_D, *green_up);
    Plane *red   = new Plane(*red_pos,   PLANE_D, *red_up  );

    floor->material = wall_checker;
    ceil->material  = wall_white;
    back->material  = wall_white;
    green->material = wall_green;
    red->material   = wall_red;

    Mirror *mirror_mat = new Mirror();
    Dielectric *glass_mat = new Dielectric(1.5f);

    float sphere_left_radius = 2.0f;
    Sphere *sphere_left = new Sphere(Vector(2.0f, sphere_left_radius, 0.8f), sphere_left_radius);
    sphere_left->material = mirror_mat;

    float sphere_right_radius = 1.7f;
    Sphere *sphere_right = new Sphere(Vector(-2.0f, sphere_right_radius, -0.2f), sphere_right_radius);
    sphere_right->material = glass_mat;

    // Compose Scene
    vector<Object3D*> objs {
        sphere_right, 
        sphere_left,
        floor,
        ceil,
        back,
        green,
        red,
        light
    };
    vector<Light*> lights {light};

    Colour void_colour = Colour(0.30f, 0.65f, 1.0f);

    return Scene(camera, objs, lights, true);

}