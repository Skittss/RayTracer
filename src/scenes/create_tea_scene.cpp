#include "create_tea_scene.h"

CSG* create_bowl(const Vector pos, const float r, const float thickness, const float ior) {

	Dielectric *glass_mat = new Dielectric(ior);
	Mirror *mirror_mat = new Mirror(Colour(0.8f), Colour(0.2f));

	Phong *bowl_mat = new Phong(
		Colour(0.0f, 0.0f, 0.0f),
		Colour(0.95f, 0.95f, 0.95f),
		Colour(1.0f, 1.0f, 1.0f),
		40.0f
	);

	CSG *bowl_csg = new CSG();

	Vector midpos = pos + Vector(0.0f, r, 0.0f);
	const float r_inner = r - thickness;
	const float d = r * 2.0f;

	Cuboid *half_cube = new Cuboid(
		midpos + Vector(0.0f, - (r / 2.0f) * 1.1f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		r,
		d + 0.1f,
		d + 0.1f
	);
	Sphere *outer = new Sphere(pos + Vector(0.0f, r, 0.0f), r);
	Sphere *inner = new Sphere(pos + Vector(0.0f, r, 0.0f), r_inner);
	half_cube->material = bowl_mat;
	outer->material = bowl_mat;
	inner->material = bowl_mat;

	CSG::csg_tree_node *half_sphere = CSG::compose_obj_pair(outer, half_cube, CSGType::INTERSECTION);
	CSG::csg_tree_node *bowl = CSG::compose_obj_pair(half_sphere, inner, CSGType::DIFFERENCE);

	bowl_csg->csg_tree_root = half_sphere;

	return bowl_csg;
}

Scene create_tea_scene() {

	float PLANE_R = 6.0f;
    float PLANE_D = 2.0f * PLANE_R;
    float SEAM_OFFSET = 0.01f;
    Camera *camera = new Camera(
		CONFIG::WIDTH, 
		CONFIG::HEIGHT, 
		66,
		CONFIG::CAMERA_FOCAL_LENGTH,
		Vector(0.0f, PLANE_R, 2.5f * PLANE_R)
	);
    camera->look_at(Vector(0.0f, PLANE_R, 0.0f));


	// LIGHTS

	SquareLight *light = new SquareLight(
        Vector(0.0f, PLANE_D - 0.1f, 0.0f),
        Vector(0.0f, -1.0f, 0.0f),
		Colour(1.0f, 1.0f, 1.0f),
		7.0f,
		2.0f,
        16.0f
    );

	// PointLight *light = new PointLight(
	// 	Vector(0.0f, PLANE_D - 2.0f, 0.0f),
	// 	// Colour(1.0f, 0.9f, 0.7f),
    //     Colour(1.0f),
	// 	16.0f,
	// 	4.0f
	// );

	// SquareLight *shoji_diffuse = new SquareLight(
	// 	Vector(0.0f, PLANE_R, PLANE_R + 0.1f),
	// 	Vector(0.0f, 0.0f, -1.0f),
	// 	Colour(1.0f),
	// 	7.0f,
	// 	6.0f,
	// 	16.0f
	// );

	//----------------------------------------------------ROOM

	//====================================================Materials
	Lambertian *wall_red = new Lambertian(
        Colour(1.0f, 0.43f, 0.41f)
    );

    Lambertian *wall_green = new Lambertian(
        Colour(0.74f, 0.90f, 0.69f)
    );

    Lambertian *wall_white = new Lambertian(
		Colour(1.0f, 1.0f, 1.0f)
    );

	Lambertian *lambertian_cream = new Lambertian(
		Colour(1.0f, 0.97647f, 0.92941f)
	);

	Lambertian *wall_checker = new Lambertian(
        Colour(1.0f, 1.0f, 1.0f)
    );
	wall_checker->apply_texture(new CheckerTexture(
		Colour(0.1f, 0.1f, 0.1f),
		Colour(0.9f, 0.9f, 0.9f),
		1.0f, 1.0f
	));

	//====================================================Geometry
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

    floor->material = lambertian_cream;
    ceil->material  = lambertian_cream;
    back->material  = lambertian_cream;
    green->material = wall_green;
    red->material   = wall_red;

	//----------------------------------------------------TABLE
	//====================================================Materials

	Lambertian *darkwood = new Lambertian(
		Colour(0.51f, 0.30f, 0.25f)
	);

	//====================================================Geometry
	const Vector table_pos = Vector(0.0f, 0.0f, 0.0f);
	const float table_size = 2.0f;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Legs
	const float leg_thickness = 0.5f;
	const float leg_height = 1.2f;
	const float leg_h_2 = leg_height / 2.0f;
	Cuboid *leg1 = new Cuboid(table_pos + Vector(-table_size, leg_h_2, -table_size), Vector(0.0f, 1.0f, 0.0f), leg_height, leg_thickness, leg_thickness);
	Cuboid *leg2 = new Cuboid(table_pos + Vector( table_size, leg_h_2, -table_size), Vector(0.0f, 1.0f, 0.0f), leg_height, leg_thickness, leg_thickness);
	Cuboid *leg3 = new Cuboid(table_pos + Vector( table_size, leg_h_2,  table_size), Vector(0.0f, 1.0f, 0.0f), leg_height, leg_thickness, leg_thickness);
	Cuboid *leg4 = new Cuboid(table_pos + Vector(-table_size, leg_h_2,  table_size), Vector(0.0f, 1.0f, 0.0f), leg_height, leg_thickness, leg_thickness);

	leg1->material = darkwood;
	leg2->material = darkwood;
	leg3->material = darkwood;
	leg4->material = darkwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Cross Beams
	const float h_ratio = 0.5f;
	const float v_thickness = 0.2f;
	const float beam_lip_d = 0.8f;
	const float beam_length = 2.0f * table_size + beam_lip_d;

	Cuboid *beam1 = new Cuboid(table_pos + Vector(-table_size, leg_h_2, 0.0f), Vector(0.0f, 1.0f, 0.0f), v_thickness, leg_thickness, beam_length);
	Cuboid *beam2 = new Cuboid(table_pos + Vector( table_size, leg_h_2, 0.0f), Vector(0.0f, 1.0f, 0.0f), v_thickness, leg_thickness, beam_length);
	Cuboid *beam3 = new Cuboid(table_pos + Vector(0.0f, leg_h_2, -table_size), Vector(0.0f, 1.0f, 0.0f), v_thickness, beam_length, leg_thickness);
	Cuboid *beam4 = new Cuboid(table_pos + Vector(0.0f, leg_h_2,  table_size), Vector(0.0f, 1.0f, 0.0f), v_thickness, beam_length, leg_thickness);

	beam1->material = darkwood;
	beam2->material = darkwood;
	beam3->material = darkwood;
	beam4->material = darkwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Top
	const float table_top_lip_d = 1.0f;
	const float table_top_thickness = 0.1f;
	const float table_top_size = 2.0f * table_size + leg_thickness / 2.0f + table_top_lip_d;

	Cuboid *table_top = new Cuboid(table_pos + Vector(0.0f, leg_height,  0.0f), Vector(0.0f, 1.0f, 0.0f), table_top_thickness, table_top_size, table_top_size);

	table_top->material = darkwood;

	//====================================================Compose Table CSG
	CSG *table_csg = new CSG();

	// Compose Legs
	CSG::csg_tree_node *leg12   = CSG::compose_obj_pair(leg1, leg2, CSGType::UNION);
	CSG::csg_tree_node *leg34   = CSG::compose_obj_pair(leg3, leg4, CSGType::UNION);
	CSG::csg_tree_node *leg1234 = CSG::compose_obj_pair(leg12, leg34, CSGType::UNION);

	// Compose Beams
	CSG::csg_tree_node *beam12   = CSG::compose_obj_pair(beam1, beam2, CSGType::UNION);
	CSG::csg_tree_node *beam34   = CSG::compose_obj_pair(beam3, beam4, CSGType::UNION);
	CSG::csg_tree_node *beam1234 = CSG::compose_obj_pair(beam12, beam34, CSGType::UNION);

	CSG::csg_tree_node *legs_and_beams = CSG::compose_obj_pair(beam1234, leg1234, CSGType::UNION);
	CSG::csg_tree_node *table_comp = CSG::compose_obj_pair(legs_and_beams, table_top, CSGType::UNION);

	table_csg->csg_tree_root = table_comp;


	//----------------------------------------------------Table Top
	//====================================================Materials

	// Lambertian *teapot_mat = new Lambertian(
	// 	Colour(0.95f, 0.95f, 0.95f)
	// );
	Phong *teapot_mat = new Phong(
		Colour(0.0f, 0.0f, 0.0f),
		Colour(0.95f, 0.95f, 0.95f),
		Colour(1.0f, 1.0f, 1.0f),
		40.0f
	);

	Dielectric *glass_mat = new Dielectric(
		1.5f
	);

	Lambertian *tray_mat = new Lambertian(
		Colour(0.784f, 0.596f, 0.471f)
	);

	//====================================================Geometry
	const float table_top_y = leg_height + 0.5 * table_top_thickness;
	const Vector table_top_origin = table_pos + Vector(0.0f, leg_height + 0.5 * table_top_thickness, 0.0f);
	const float table_top_rad = table_top_size / 2.0f;

	const Vector teapot_pos = table_top_origin + Vector(table_top_rad * 0.3f, 0.0f, table_top_rad * 0.3f);

	const Vector tray_pos = table_top_origin + Vector(-table_top_rad * 0.3f, 0.0f, -table_top_rad * 0.0f);
	const float tray_depth = 1.3f;
	const float tray_width = 2.7f;
	const float tray_height = 0.2f;

	const float sphere_radius = 0.8f;
	const Vector sphere_pos = table_top_origin + Vector(-table_top_rad * 0.3f, tray_height, -table_top_rad * 0.3f);

	Transform teapot_transform = Transform(
		0.707107f, 	0.707107f, 	0.0f, teapot_pos.x,
		0.0f, 		0.0f, 		1.0f, teapot_pos.y,
		-0.707107f, 0.707107f, 	0.0f, teapot_pos.z,
		0.0f, 		0.0f, 		0.0f, 0.5f
	);

	PolyMesh *teapot = new PolyMesh(CONFIG::TEAPOT_PATH, teapot_transform, true);

	teapot->material = teapot_mat;


	Cuboid *tray = new Cuboid(
		tray_pos,
		Vector(0.0f, 1.0f, 0.0f),
		Vector::norm(Vector(1.0f, 0.0f, 1.0f)),
		tray_height,
		tray_depth,
		tray_width
	);
	tray->material = tray_mat;

	const float bowl_thickness = 0.1f;
	const float bowl_rad = 0.5f;
	CSG *bowl1 = create_bowl(sphere_pos, bowl_rad, bowl_thickness, 1.0f);

	// Sphere *glass_sphere = new Sphere(sphere_pos, sphere_radius);
	
	// glass_sphere->material = glass_mat;

	//----------------------------------------------------Caustic Balls
	//====================================================Materials
	Dielectric *glass_ball_mat = new Dielectric(1.5f);
	Mirror *mirror_ball_mat = new Mirror();

	//====================================================Geometry
	const float caustic_sphere_r = 1.5f;
	const float sphere_center_offset = 0.65f;

	Sphere *glass_ball = new Sphere(
		Vector(PLANE_R * sphere_center_offset, caustic_sphere_r + 0.05f, 0.0f),
		caustic_sphere_r
	);

	Sphere *mirror_ball = new Sphere(
		Vector(-PLANE_R * sphere_center_offset, caustic_sphere_r + 0.05f, PLANE_R * sphere_center_offset),
		caustic_sphere_r
	);

	glass_ball->material = glass_ball_mat;
	mirror_ball->material = mirror_ball_mat;

	//----------------------------------------------------Window
	//====================================================Materials
	Lambertian *sandwood = new Lambertian(
		Colour(0.784f, 0.596f, 0.471f)
	);

	//====================================================Geometry
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Bezel
	const float window_outer_radius = 3.0f;
	const float window_bezel_thickness = 0.3f;
	const float window_inner_radius = window_outer_radius - window_bezel_thickness;
	const float window_bezel_lip_d = 0.3f;
	const float bound_dst = 2.0f * window_outer_radius + 0.2f;

	Vector window_center = Vector(0.0f, PLANE_R, PLANE_R - SEAM_OFFSET);
	Sphere *outer_sphere = new Sphere(window_center, window_outer_radius);
	Sphere *inner_sphere = new Sphere(window_center, window_inner_radius);
	Cuboid *bezel_cut_cube = new Cuboid(window_center, Vector(0.0f, 1.0f, 0.0f), bound_dst, bound_dst, window_bezel_lip_d);

	outer_sphere->material = darkwood;
	inner_sphere->material = darkwood;
	bezel_cut_cube->material = darkwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Beams
	const float beam_lip_offset = window_bezel_lip_d / 2.0f;
	const float beam_lip_thickness = window_bezel_lip_d / 4.0f;
	const float win_diameter = window_outer_radius * 2.0f;

	// Horizontal Beams
	// Top
	Cuboid *win_beam1 = new Cuboid(
		window_center - Vector(-win_diameter / 7.0f, win_diameter / 20.0f, beam_lip_offset + beam_lip_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f), 
		win_diameter / 30.0f, win_diameter, beam_lip_thickness
	);
	// Bottom
	Cuboid *win_beam2 = new Cuboid(
		window_center - Vector(0.0f, win_diameter / 5.0f, beam_lip_offset + beam_lip_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f), 
		win_diameter / 20.0f, win_diameter + 0.4f, beam_lip_thickness
	);

	// Vertical Beams
	// Left
	Cuboid *win_beam3 = new Cuboid(
		window_center - Vector(win_diameter / 20.0f, -win_diameter / 5.0f, beam_lip_offset + beam_lip_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f), 
		win_diameter, win_diameter / 35.0f, beam_lip_thickness
	);
	// Middle
	Cuboid *win_beam4 = new Cuboid(
		window_center - Vector(win_diameter / 6.0f, 0.0f, beam_lip_offset + beam_lip_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f), 
		win_diameter + 0.4f, win_diameter / 20.0f, beam_lip_thickness
	);
	// Right
	Cuboid *win_beam5 = new Cuboid(
		window_center - Vector(win_diameter / 3.5f, win_diameter / 10.0f, beam_lip_offset + beam_lip_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f), 
		win_diameter / 2.5f, win_diameter / 35.0f, beam_lip_thickness
	);

	win_beam1->material = sandwood;
	win_beam2->material = sandwood;
	win_beam3->material = sandwood;
	win_beam4->material = sandwood;
	win_beam5->material = sandwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Compose Window CSG

	CSG *window_csg = new CSG();

	CSG::csg_tree_node *win_beams12 = CSG::compose_obj_pair(win_beam1, win_beam2, CSGType::UNION);
	CSG::csg_tree_node *win_beams34 = CSG::compose_obj_pair(win_beam3, win_beam4, CSGType::UNION);
	CSG::csg_tree_node *win_beams1234 = CSG::compose_obj_pair(win_beams12, win_beams34, CSGType::UNION);
	CSG::csg_tree_node *win_beams = CSG::compose_obj_pair(win_beams1234, win_beam5, CSGType::UNION);

	CSG::csg_tree_node *clipped_win_beams = CSG::compose_obj_pair(win_beams, outer_sphere, CSGType::INTERSECTION);

	CSG::csg_tree_node *bezel_segment = CSG::compose_obj_pair(outer_sphere, bezel_cut_cube, CSGType::INTERSECTION);
	CSG::csg_tree_node *bezel = CSG::compose_obj_pair(bezel_segment, inner_sphere, CSGType::DIFFERENCE);
	CSG::csg_tree_node *bezel_and_beams = CSG::compose_obj_pair(bezel, clipped_win_beams, CSGType::UNION);

	window_csg->csg_tree_root = bezel_and_beams;

	// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Cut Wall Hole
	// const float wall_replacement_width = 0.2f;
	// Cuboid *back_wall_cuboid = new Cuboid(
	// 	Vector(0.0f, PLANE_R, PLANE_R - SEAM_OFFSET + wall_replacement_width),
	// 	Vector(0.0f, 1.0f, 0.0f),
	// 	PLANE_D,
	// 	PLANE_D,
	// 	wall_replacement_width
	// );
	// back_wall_cuboid->material = lambertian_cream;

	// CSG *cut_wall_back = new CSG;
	// CSG::csg_tree_node *cut_wall = CSG::compose_obj_pair(back_wall_cuboid, inner_sphere, CSGType::DIFFERENCE);

	// cut_wall_back->csg_tree_root = cut_wall;

	//----------------------------------------------------Wall Beams
	//====================================================Geometry
	const float wall_beam_parallel_thickness = 0.8f;
	const float wall_beam_perpendicular_thickness = 0.25f;

	const float wall_beam_horizontal_dst_prop = 0.83f;
	const float wall_beam_horizontal_thickness = 0.5f;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Vertical Beams
	// Back Wall
	Cuboid *wall_beam_br = new Cuboid(
		Vector(-PLANE_R + wall_beam_parallel_thickness / 2.0f, PLANE_R, PLANE_R - SEAM_OFFSET),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_parallel_thickness,
		wall_beam_perpendicular_thickness
	);
	Cuboid *wall_beam_bl = new Cuboid(
		Vector(PLANE_R - wall_beam_parallel_thickness / 2.0f, PLANE_R, PLANE_R - SEAM_OFFSET),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_parallel_thickness,
		wall_beam_perpendicular_thickness
	);

	// Left Wall
	Cuboid *wall_beam_lr = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET, PLANE_R, PLANE_R - wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_perpendicular_thickness,
		wall_beam_parallel_thickness
	);
	Cuboid *wall_beam_ll = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET, PLANE_R, -PLANE_R + wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_perpendicular_thickness,
		wall_beam_parallel_thickness
	);

	// Right Wall
	Cuboid *wall_beam_rr = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET, PLANE_R, PLANE_R - wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_perpendicular_thickness,
		wall_beam_parallel_thickness
	);
	Cuboid *wall_beam_rl = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET, PLANE_R, -PLANE_R + wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		PLANE_D + 0.1f,
		wall_beam_perpendicular_thickness,
		wall_beam_parallel_thickness
	);

	wall_beam_br->material = sandwood;
	wall_beam_bl->material = sandwood;
	wall_beam_lr->material = sandwood;
	wall_beam_ll->material = sandwood;
	wall_beam_rr->material = sandwood;
	wall_beam_rl->material = sandwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Horizontal Beams
	// Back Wall Horizontal
	Cuboid *wall_beam_hb = new Cuboid(
		Vector(0.0f, PLANE_D * wall_beam_horizontal_dst_prop, PLANE_R - SEAM_OFFSET),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_horizontal_thickness,
		PLANE_D + 0.1f,
		wall_beam_perpendicular_thickness
	);
	// Left Wall Horizontal
	Cuboid *wall_beam_hl = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET, PLANE_D * wall_beam_horizontal_dst_prop, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_horizontal_thickness,
		wall_beam_perpendicular_thickness,
		PLANE_D + 0.1f
	);
	// Right Wall Horizontal
	Cuboid *wall_beam_hr = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET, PLANE_D * wall_beam_horizontal_dst_prop, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_horizontal_thickness,
		wall_beam_perpendicular_thickness,
		PLANE_D + 0.1f
	);
	
	wall_beam_hb->material = sandwood;
	wall_beam_hl->material = sandwood;
	wall_beam_hr->material = sandwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Ceiling Beams
	Cuboid *ceiling_beam_b = new Cuboid(
		Vector(0.0f, PLANE_D - SEAM_OFFSET, PLANE_R - SEAM_OFFSET - wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_perpendicular_thickness,
		PLANE_D + 0.1f,
		wall_beam_horizontal_thickness
	);
	Cuboid *ceiling_beam_f = new Cuboid(
		Vector(0.0f, PLANE_D - SEAM_OFFSET, -PLANE_R + SEAM_OFFSET + wall_beam_parallel_thickness / 2.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_perpendicular_thickness,
		PLANE_D + 0.1f,
		wall_beam_horizontal_thickness
	);
	Cuboid *ceiling_beam_l = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET - wall_beam_parallel_thickness / 2.0f, PLANE_D - SEAM_OFFSET, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_perpendicular_thickness,
		wall_beam_horizontal_thickness,
		PLANE_D + 0.1f
	);
	Cuboid *ceiling_beam_r = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET + wall_beam_parallel_thickness / 2.0f, PLANE_D - SEAM_OFFSET, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		wall_beam_perpendicular_thickness,
		wall_beam_horizontal_thickness,
		PLANE_D + 0.1f
	);

	ceiling_beam_b->material = sandwood;
	ceiling_beam_f->material = sandwood;
	ceiling_beam_l->material = sandwood;
	ceiling_beam_r->material = sandwood;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Compose Wall Beam CSG
	CSG *wall_beams = new CSG();
	
	CSG::csg_tree_node *back_wall_beams = CSG::compose_obj_pair(wall_beam_br, wall_beam_bl, CSGType::UNION);
	CSG::csg_tree_node *left_wall_beams = CSG::compose_obj_pair(wall_beam_lr, wall_beam_ll, CSGType::UNION);
	CSG::csg_tree_node *right_wall_beams = CSG::compose_obj_pair(wall_beam_rr, wall_beam_rl, CSGType::UNION);

	CSG::csg_tree_node *bl_wall_beams = CSG::compose_obj_pair(back_wall_beams, left_wall_beams, CSGType::UNION);
	CSG::csg_tree_node *vertical_wall_beams = CSG::compose_obj_pair(bl_wall_beams, right_wall_beams, CSGType::UNION);

	CSG::csg_tree_node *wall_beam_h_bl = CSG::compose_obj_pair(wall_beam_hb, wall_beam_hl, CSGType::UNION);
	CSG::csg_tree_node *horizontal_wall_beams = CSG::compose_obj_pair(wall_beam_h_bl, wall_beam_hr, CSGType::UNION);

	CSG::csg_tree_node *all_wall_beams = CSG::compose_obj_pair(horizontal_wall_beams, vertical_wall_beams, CSGType::UNION);

	CSG::csg_tree_node *ceiling_beam_bf = CSG::compose_obj_pair(ceiling_beam_b, ceiling_beam_f, CSGType::UNION);
	CSG::csg_tree_node *ceiling_beam_lr = CSG::compose_obj_pair(ceiling_beam_l, ceiling_beam_r, CSGType::UNION);
	CSG::csg_tree_node *all_ceiling_beams = CSG::compose_obj_pair(ceiling_beam_bf, ceiling_beam_lr, CSGType::UNION);

	CSG::csg_tree_node *all_beams = CSG::compose_obj_pair(all_wall_beams, all_ceiling_beams, CSGType::UNION);

	wall_beams->csg_tree_root = all_beams;

	//----------------------------------------------------Tatami Mat Dividers
	// Layout:
	//  ___________
	// |_____|_____|
	// |  |_____|  |
	// |__|_____|__|
	// |_____|_____|
	//
	//====================================================Materials
	Lambertian *tatami_divider_mat = new Lambertian(
		Colour(0.16471f, 0.17255f, 0.12941f)
	);

	//====================================================Geometry
	const float tatami_divider_extrusion = 0.05f;
	const float tatami_divider_width = 0.2f;
	const float tatami_unit_width = PLANE_D / 4.0f;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Horizontal Dividers
	Cuboid *h_div_1 = new Cuboid(
		Vector(0.0f, 0.0f, PLANE_R - SEAM_OFFSET),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		PLANE_D + 0.1f,
		tatami_divider_width
	);
	Cuboid *h_div_2 = new Cuboid(
		Vector(0.0f, 0.0f, PLANE_R - SEAM_OFFSET - tatami_unit_width),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		PLANE_D + 0.1f,
		tatami_divider_width
	);
	Cuboid *h_div_3 = new Cuboid(
		Vector(0.0f, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_unit_width * 2.0f,
		tatami_divider_width
	);
	Cuboid *h_div_4 = new Cuboid(
		Vector(0.0f, 0.0f, -PLANE_R + SEAM_OFFSET + tatami_unit_width),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		PLANE_D + 0.1f,
		tatami_divider_width
	);
	Cuboid *h_div_5 = new Cuboid(
		Vector(0.0f, 0.0f, -PLANE_R + SEAM_OFFSET),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		PLANE_D + 0.1f,
		tatami_divider_width
	);

	h_div_1->material = tatami_divider_mat;
	h_div_2->material = tatami_divider_mat;
	h_div_3->material = tatami_divider_mat;
	h_div_4->material = tatami_divider_mat;
	h_div_5->material = tatami_divider_mat;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Vertical Dividers
	Cuboid *v_div_1 = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		PLANE_D + 0.1f
	);
	Cuboid *v_div_2 = new Cuboid(
		Vector(PLANE_R - SEAM_OFFSET - tatami_unit_width, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		tatami_unit_width * 2.0f
	);
	Cuboid *v_div_3 = new Cuboid(
		Vector(0.0f, 0.0f, PLANE_R - SEAM_OFFSET - 0.5f * tatami_unit_width),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		tatami_unit_width
	);
	Cuboid *v_div_4 = new Cuboid(
		Vector(0.0f, 0.0f, -PLANE_R + SEAM_OFFSET + 0.5f * tatami_unit_width),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		tatami_unit_width
	);
	Cuboid *v_div_5 = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET + tatami_unit_width, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		tatami_unit_width * 2.0f
	);
	Cuboid *v_div_6 = new Cuboid(
		Vector(-PLANE_R + SEAM_OFFSET, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		tatami_divider_extrusion,
		tatami_divider_width,
		PLANE_D + 0.1f
	);

	v_div_1->material = tatami_divider_mat;
	v_div_2->material = tatami_divider_mat;
	v_div_3->material = tatami_divider_mat;
	v_div_4->material = tatami_divider_mat;
	v_div_5->material = tatami_divider_mat;
	v_div_6->material = tatami_divider_mat;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Compose Tatami Mat Dividers CSG
	CSG *tatami_dividers = new CSG();

	CSG::csg_tree_node *dividers_h12 = CSG::compose_obj_pair(h_div_1, h_div_2, CSGType::UNION);
	CSG::csg_tree_node *dividers_h34 = CSG::compose_obj_pair(h_div_3, h_div_4, CSGType::UNION);
	CSG::csg_tree_node *dividers_h1234 = CSG::compose_obj_pair(dividers_h12, dividers_h34, CSGType::UNION);
	CSG::csg_tree_node *h_dividers = CSG::compose_obj_pair(dividers_h1234, h_div_5, CSGType::UNION);

	CSG::csg_tree_node *dividers_v12 = CSG::compose_obj_pair(v_div_1, v_div_2, CSGType::UNION);
	CSG::csg_tree_node *dividers_v34 = CSG::compose_obj_pair(v_div_3, v_div_4, CSGType::UNION);
	CSG::csg_tree_node *dividers_v56 = CSG::compose_obj_pair(v_div_5, v_div_6, CSGType::UNION);
	CSG::csg_tree_node *dividers_v1234 = CSG::compose_obj_pair(dividers_v12, dividers_v34, CSGType::UNION);
	CSG::csg_tree_node *v_dividers = CSG::compose_obj_pair(dividers_v1234, dividers_v56, CSGType::UNION);

	CSG::csg_tree_node *all_dividers = CSG::compose_obj_pair(h_dividers, v_dividers, CSGType::UNION);

	tatami_dividers->csg_tree_root = all_dividers;
	
	//----------------------------------------------------Second Mat

	Cuboid *matmat = new Cuboid(
		Vector(0.0f, 0.0f, -PLANE_R * 0.9f),
		Vector(0.0f, 1.0f, 0.0f),
		Vector::norm(Vector(1.0f, 0.0f, 1.0f)),
		0.4f,
		2.5f,
		2.5f
	);
	matmat->material = sandwood;

	//----------------------------------------------------Lanterns
	//====================================================Materials

	Dielectric *lantern_glass = new Dielectric(1.3f);

	Lambertian *rope_mat = new Lambertian(
		Colour(0.16471f, 0.17255f, 0.12941f)
	);

	//====================================================Geometry
	const float lantern_rope_thickness = 0.05f;
	const float lantern_holster_width = 0.2f;
	const float lantern_holster_extrusion = 0.1f;

	const Vector lantern_pos1 = Vector(PLANE_R * 0.8f, PLANE_D - SEAM_OFFSET, PLANE_R * 0.4f);
	const Vector lantern_pos2 = Vector(-PLANE_R * 0.8f, PLANE_D - SEAM_OFFSET, -PLANE_R * 0.4f);

	const float lantern_len1 = 3.0f;
	const float lantern_len2 = 5.5f;

	const float lantern_rad1 = 1.0f;
	const float lantern_rad2 = 1.0f;

	// Holsters
	Cuboid *lantern_holster1 = new Cuboid(
		lantern_pos1,
		Vector(0.0f, 1.0f, 0.0f),
		lantern_holster_extrusion,
		lantern_holster_width,
		lantern_holster_width
	);
	Cuboid *lantern_holster2 = new Cuboid(
		lantern_pos2,
		Vector(0.0f, 1.0f, 0.0f),
		lantern_holster_extrusion,
		lantern_holster_width,
		lantern_holster_width
	);

	lantern_holster1->material = sandwood;
	lantern_holster2->material = sandwood;

	// Ropes
	Cuboid *lantern_rope_1 = new Cuboid(
		lantern_pos1 - Vector(0.0f, lantern_len1 / 2.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		lantern_len1,
		lantern_rope_thickness,
		lantern_rope_thickness
	);
	Cuboid *lantern_rope_2 = new Cuboid(
		lantern_pos2 - Vector(0.0f, lantern_len2 / 2.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		lantern_len2,
		lantern_rope_thickness,
		lantern_rope_thickness
	);

	lantern_rope_1->material = rope_mat;
	lantern_rope_2->material = rope_mat;

	// Balls
	Sphere *lantern_ball1 = new Sphere(
		lantern_pos1 - Vector(0.0f, lantern_len1 + lantern_rad1, 0.0f),
		lantern_rad1
	);

	Sphere *lantern_ball2 = new Sphere(
		lantern_pos2 - Vector(0.0f, lantern_len2 + lantern_rad2, 0.0f),
		lantern_rad2
	);

	lantern_ball1->material = lantern_glass;
	lantern_ball2->material = lantern_glass;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Compose Lantern CSG
	// Lantern 1
	CSG *lantern1 = new CSG();
	CSG::csg_tree_node *connected_rope1 = CSG::compose_obj_pair(lantern_holster1, lantern_rope_1, CSGType::UNION);
	CSG::csg_tree_node *full_lantern1 = CSG::compose_obj_pair(connected_rope1, lantern_ball1, CSGType::UNION);

	lantern1->csg_tree_root = full_lantern1;

	CSG *lantern2 = new CSG();
	CSG::csg_tree_node *connected_rope2 = CSG::compose_obj_pair(lantern_holster2, lantern_rope_2, CSGType::UNION);
	CSG::csg_tree_node *full_lantern2 = CSG::compose_obj_pair(connected_rope2, lantern_ball2, CSGType::UNION);

	lantern2->csg_tree_root = full_lantern2;


	//----------------------------------------------------COMPOSE
	vector<Object3D*> objs {
		// Room
		floor,
        ceil,
        back,
        green,
        red,
		// Tatami
		tatami_dividers,
		matmat,
		// Table
		table_csg,
		// Table top
		teapot,
		tray,
		bowl1,
		// Big Spheres
		// glass_ball,
		mirror_ball,
		// Window
		window_csg,
		// Beams
		wall_beams,
		// Lanterns
		lantern1,
		lantern2,
		// Visible Lights
        light
    };

    vector<Light*> lights {
		light
	};

	return Scene(camera, objs, lights, true);
}