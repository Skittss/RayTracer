#include "create_csg_test_scene.h"

Scene create_csg_test_scene() {

	Camera *camera = new Camera(
		CONFIG::WIDTH, 
		CONFIG::HEIGHT, 
		CONFIG::CAMERA_FOV, 
		CONFIG::CAMERA_FOCAL_LENGTH,
		Vector(5.0f, 3.0f, -5.0f)
	);

	camera->look_at(Vector(0.0f, 1.5f, 0.0f));

	Lambertian *lambertian_cream = new Lambertian(
		Colour(0.50f, 0.65f, 0.75f)
	);

	Phong *phong_mat = new Phong(
		Colour(0.00f, 0.10f, 0.15f), 1.0f,
		Colour(0.50f, 0.65f, 0.75f), 1.0f,
		Colour(0.40f, 0.40f, 0.40f), 1.0f,
		60.0f
	);

	Normat *normal_mat = new Normat();

	Cuboid *cube1 = new Cuboid(
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.7f, 0.0f, 0.3f), 
		2.0f
	);
	Lambertian *cube_mat1 = new Lambertian(
		Colour(0.7f, 0.2f, 0.2f)
	);
	cube1->material = cube_mat1;

	float cross_width = 0.7f;

	Cuboid *cube2 = new Cuboid(
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.7f, 0.0f, 0.3f), 
		cross_width, cross_width, 2.5f
	);
	Cuboid *cube3 = new Cuboid(
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.7f, 0.0f, 0.3f), 
		cross_width, 2.5f, cross_width
	);
	Cuboid *cube4 = new Cuboid(
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.0f, 1.0f, 0.0f), 
		Vector(0.7f, 0.0f, 0.3f), 
		2.5f, cross_width, cross_width
	);
	Lambertian *cube_mat2 = new Lambertian(
		Colour(0.2f, 0.7f, 0.2f)
	);
	cube2->material = cube_mat2;
	cube3->material = cube_mat2;
	cube4->material = cube_mat2;

	// Constructive solid geometry
	Sphere *csg_sphere_1 = new Sphere(Vector(0.0f, 1.0f, 0.0f), 1.35f);
	csg_sphere_1->material = lambertian_cream;

	Sphere *csg_sphere_2 = new Sphere(Vector(1.0f, 1.0f, 0.0f), 1.0f);
	csg_sphere_2->material = normal_mat;

	CSG *finished_csg = new CSG();
	CSG::csg_tree_node *smooth_cube = CSG::compose_obj_pair(cube1, csg_sphere_1, CSGType::INTERSECTION);
	CSG::csg_tree_node *cut_cube = CSG::compose_obj_pair(cube1, csg_sphere_1, CSGType::DIFFERENCE);
	CSG::csg_tree_node *cross1 = CSG::compose_obj_pair(cube2, cube3, CSGType::UNION);
	CSG::csg_tree_node *cross2 = CSG::compose_obj_pair(cube4, cross1, CSGType::UNION);

	CSG::csg_tree_node *cut_cross = CSG::compose_obj_pair(cross1, cube4, CSGType::INTERSECTION);
	CSG::csg_tree_node *combined = CSG::compose_obj_pair(smooth_cube, cross2, CSGType::DIFFERENCE);

	finished_csg->csg_tree_root = combined;

	Phong *plane_mat = new Phong(
		Colour(0.01f, 0.01f, 0.01f), 1.0f,
		Colour(1.0f, 1.0f, 1.0f), 1.0f,
		Colour(1.0f, 0.25f, 0.0f), 1.0f,
		20.0f
	);
	plane_mat->apply_texture(new CheckerTexture(
		Colour(0.1f, 0.1f, 0.1f),
		Colour(0.9f, 0.9f, 0.9f),
		1.0f, 1.0f
	));

	Plane *plane1 = new Plane(
		Vector(0.0f, 0.0f, 0.0f), 
		80.0f, 
		Vector(0.0f, 1.0f, 0.0f)
	);
	plane1->material = plane_mat;

    SquareLight *square_light = new SquareLight(
        Vector(-3.0f, 6.0f, 4.0f),
        Vector(0.0f, -1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		7.0f,
		2.0f,
        16.0f
    );

    PointLight *p_light_csg = new PointLight(
		Vector(0.0f, 1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		1.0f,
		1.0f
	);

    PointLight *p_light2 = new PointLight(
		Vector(-4.0f, 4.0f, 2.0f),
		Colour(1.0f, 1.0f, 1.0f),
		18.0f,
		5.0f
	);


	// Compose
	vector<Object3D*> objs {
        cube4
		// finished_csg
		// plane1
	};

	vector<Light*> lights {
        square_light,
		// p_light_csg
	};

	Colour void_colour = Colour(0.30f, 0.65f, 1.0f);

	return Scene(camera, objs, lights, void_colour, false);

}