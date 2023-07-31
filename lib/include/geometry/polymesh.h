#pragma once

#include "../data/bounding_volume.h"
#include "../data/transform.h"
#include "object3D.h"

typedef int TriangleIdxMap[3];

class BoundingVolume;

class PolyMesh : public Object3D {
private:

	BoundingVolume *bounding_volume;

	void display_progress(int total, int i, char *str, char *label);
	void load_file(char *file, const Transform &transform);

	Vector compute_face_normal(int triangle_idx);
	void compute_vertex_normals();

	bool moller_trumbore(
		const Ray& ray, 
		const Vector &v0, const Vector &v1, const Vector &v2, 
		float &t, float &u, float &v
	);
	bool intersect_triangle(Ray ray, Hit &hit, int triangle_idx);

public:
	int vertex_count;
	int face_count;

	Vector *vertices;
	Vector *face_normals;
	Vector *vertex_normals;
	bool interpolate_normals;

	TriangleIdxMap *triangles;

	void compute_bounds(Vector plane_norm, float &near, float &far);
	vector<Hit> intersection(Ray ray) override;

	PolyMesh(char *filepath);
	PolyMesh(char *filepath, const Transform &transform, bool smooth);
};
