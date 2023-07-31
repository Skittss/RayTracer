#include "create_test_scene.h"

Scene create_test_scene() {

	Camera *camera = new Camera(
		CONFIG::WIDTH, 
		CONFIG::HEIGHT, 
		CONFIG::CAMERA_FOV, 
		CONFIG::CAMERA_FOCAL_LENGTH,
		Vector(3.0f, 1.7f, -3.0f)
	);

	camera->look_at(Vector(0.0f, 1.5f, 0.0f));

	// Homogeneous coordinates to transform the teapot geometry into view.
	Transform teapot_transform = Transform(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	Lambertian *teapot_mat = new Lambertian(
		Colour(0.50f, 0.65f, 0.75f)
	);

	Phong *phong_mat = new Phong(
		Colour(0.00f, 0.10f, 0.15f), 1.0f,
		Colour(0.50f, 0.65f, 0.75f), 1.0f,
		Colour(0.40f, 0.40f, 0.40f), 1.0f,
		60.0f
	);

	Phong *phong_red = new Phong(
		Colour(0.1f, 0.0f, 0.0f), 1.0f,
		Colour(1.0f, 0.43f, 0.41f), 1.0f,
		Colour(1.0f, 1.0f, 1.0f), 1.0f,
		20.0f
	);

	Normat *normal_mat = new Normat();

	// Teapot from ply file
	PolyMesh *teapot = new PolyMesh((char *)CONFIG::TEAPOT_PATH, teapot_transform, false);

	teapot->material = teapot_mat;
	// teapot->material = normal_mat;

	// Spheres
	Mirror *mirror1 = new Mirror();
	Mirror *mirror2 = new Mirror(Colour(0.9f, 0.43, 0.41));
	Dielectric *sphere_mat = new Dielectric(1.0f);

	Sphere *sphere1 = new Sphere(Vector(0.0f, 1.05f, 0.0f), 1.0f);
	Sphere *sphere3 = new Sphere(Vector(0.0f, 1.05f, 1.5f), 1.0f);
	Sphere *sphere2 = new Sphere(Vector(0.0f, 1.0f, 0.0f), 0.5f);

	Sphere *sphere4 = new Sphere(Vector(6.0f, 1.05f, -1.5f), 1.0f);
	Sphere *sphere5 = new Sphere(Vector(6.0f, 1.05f, 1.5f), 1.0f);

	Sphere *sphere6 = new Sphere(Vector(-4.0f, 1.05f, -1.5f), 1.0f);
	Sphere *sphere7 = new Sphere(Vector(-4.0f, 1.05f, 1.5f), 1.0f);
	Quadric *q1 = new Quadric(UNIT_QUADRIC::ELLIPSE, Vector(0.0f, 1.0f, 0.0f), 1.0f, 2.0f, 1.0f, 1.0f, 0.5f);

	sphere1->material = phong_red;
	sphere2->material = sphere_mat;
	sphere3->material = mirror2;
	sphere4->material = mirror2;
	sphere5->material = mirror1;
	sphere6->material = mirror2;
	sphere7->material = mirror1;
	q1->material = phong_red;

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
	csg_sphere_1->material = teapot_mat;

	Sphere *csg_sphere_2 = new Sphere(Vector(1.0f, 1.0f, 0.0f), 1.0f);
	csg_sphere_2->material = normal_mat;

	CSG *csg_obj = new CSG();
	CSG::csg_tree_node *smooth_cube = CSG::compose_obj_pair(cube1, csg_sphere_1, CSGType::INTERSECTION);
	CSG::csg_tree_node *cut_cube = CSG::compose_obj_pair(cube1, csg_sphere_1, CSGType::DIFFERENCE);
	CSG::csg_tree_node *cross1 = CSG::compose_obj_pair(cube2, cube3, CSGType::UNION);
	CSG::csg_tree_node *cross2 = CSG::compose_obj_pair(cube4, cross1, CSGType::UNION);

	CSG::csg_tree_node *cut_cross = CSG::compose_obj_pair(cross1, cube4, CSGType::INTERSECTION);

	CSG::csg_tree_node *combined = CSG::compose_obj_pair(smooth_cube, cross2, CSGType::DIFFERENCE);

	csg_obj->csg_tree_root = combined;

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

	// // Lights
	// DirectionalLight *d_light1 = new DirectionalLight(
	// 	Vector(-1.0f, -1.0f, 1.0f),
	// 	Colour(1.0f, 1.0f, 1.0f),
	// 	1.0f
	// );

	// DirectionalLight *d_light2 = new DirectionalLight(
	// 	Vector(1.0f, -1.0f, 1.0f),
	// 	Colour(1.0f, 1.0f, 1.0f),
	// 	1.0f
	// );

	PointLight *p_light1 = new PointLight(
		Vector(2.0f, 4.0f, -4.0f),
		Colour(1.0f, 1.0f, 1.0f),
		20.0f,
		5.0f
	);

	PointLight *p_light_csg = new PointLight(
		Vector(0.0f, 1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		1.0f,
		1.0f
	);

	PointLight *p_light2 = new PointLight(
		Vector(-4.0f, 4.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		6.0f,
		5.0f
	);

	PointLight *p_light3 = new PointLight(
		Vector(4.0f, 4.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		6.0f,
		5.0f
	);

	SquareLight *a_light = new SquareLight(
        Vector(4.0f, 4.0f, 0.0f),
        Vector(0.0f, -1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		7.0f,
		4.0f,
        16.0f
    );

		SquareLight *a_light2 = new SquareLight(
        Vector(-4.0f, 4.0f, 0.0f),
        Vector(0.0f, -1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		7.0f,
		4.0f,
        16.0f
    );

	

	// Compose
	vector<Object3D*> objs {
		// teapot,
		// q1,
		// cube1,
		// csg_obj,
		sphere1,
		// sphere2,
		// sphere3,
		// sphere4,
		// sphere5,
		// sphere6,
		// sphere7,
		plane1
	};

	vector<Light*> lights {
		// p_light1, 
		// p_light_csg,
		p_light2,
		// p_light3
	};

	Colour void_colour = Colour(0.30f, 0.65f, 1.0f);
	// Colour void_colour = Colour(0.0f);

	return Scene(camera, objs, lights, void_colour, false);

}