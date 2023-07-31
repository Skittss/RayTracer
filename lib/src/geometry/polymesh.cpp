#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include <geometry/polymesh.h>

using namespace std;

PolyMesh::PolyMesh(char *filepath) : PolyMesh(filepath, Transform(), true) {}

PolyMesh::PolyMesh(char *filepath, const Transform &transform, bool smooth) {
	interpolate_normals = smooth;
  	load_file(filepath, transform);
}

// CLI Loading progress indicator.
void PolyMesh::display_progress(int total, int i, char *counter, char *label) {
	
	float current = i;
	float total_f = total;
	float progress = current / total_f;
	const int bar_width = 70;

	cerr << "  [";
	int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) cerr << "=";
        else if (i == pos) cerr << ">";
        else cerr << " ";
    }
	cerr << "] " << int(progress * 100.0) << " % (" << current << "/" << total << " " << counter << ") " << label << "\r";
    cerr.flush();
}

/// @brief Load a polymesh from a .ply file
/// @param filepath .ply filepath
/// @param transform Homogenous transform to apply to polymesh on load.
void PolyMesh::load_file(char *filepath, const Transform &transform) {

	int linecount = 1;
	ifstream meshfile(filepath);

	cerr << "Importing \"" << filepath << "\"" << endl;

	if (!meshfile.is_open()) {
		std::ostringstream err;
		err << "ERR: The specified file \"" << filepath << "\" could not be found." << endl;
		meshfile.close();
		std::logic_error(err.str());
	}

	string line;

	// Read ply file format (line 1).
	try {
		getline(meshfile, line);
		linecount++;
	} catch(ifstream::failure e) {
		std::ostringstream err;
		err << "ERR: Could not read line (" << linecount << ") from file \"" << filepath << "\"." << endl;
		std::logic_error(err.str());
	}
	if (line.compare("kcply") != 0) {
		std::ostringstream err;
		err << "ERR: Incompatible mesh format. File: \"" << filepath << "\" Expected: kcply. Received: " << line << endl;
		meshfile.close();
		std::logic_error(err.str());
	}

	// Read vertex count (line 2).
	try {
		getline(meshfile, line);
		linecount++;
	} catch(ifstream::failure e) {
		std::ostringstream err;
		err << "ERR: Could not read line (" << linecount << ") from file \"" << filepath << "\"." << endl;
		std::logic_error(err.str());
	}

	istringstream vertex_line_stream(line);
	string vertex_qualifier;
	string vertex_label;

	vertex_line_stream >> vertex_qualifier >> vertex_label >> vertex_count;

	if ((vertex_qualifier.compare("element") != 0) || (vertex_label.compare("vertex") != 0)) {
		std::ostringstream err;
		err << "ERR: Could not read vertex count from file \"" << filepath << "\" Line (" << linecount << ")." << endl;
		meshfile.close();
		std::logic_error(err.str());
	}

	// Read face count (line 3).
	try {
		getline(meshfile, line);
		linecount++;
	} catch(ifstream::failure e) {
		std::ostringstream err;
		err << "ERR: Could not read line (" << linecount << ") from file \"" << filepath << "\"." << endl;
		std::logic_error(err.str());
	}

	istringstream face_line_stream(line);
	string face_qualifier;
	string face_label;

	face_line_stream >> face_qualifier >> face_label >> face_count;

	if ((face_qualifier.compare("element") != 0) || (face_label.compare("face") != 0)) {
		std::ostringstream err;
		err << "ERR: Could not read face count from file \"" << filepath << "\" Line (" << linecount << ")." << endl;
		meshfile.close();
		std::logic_error(err.str());
	}

	// Read vertices.
	vertices = new Vector[vertex_count];

	char *vertex_counter_txt = (char *)"vertices";
	char *vertex_progress_label = (char *)"Reading vertices...";

	display_progress(vertex_count, 0, vertex_counter_txt, vertex_progress_label);
	for (int i = 0; i < vertex_count; i++)
	{
		try {
			getline(meshfile, line);
			linecount++;
		} catch(ifstream::failure e) {
			std::ostringstream err;
			err << endl << "ERR: Could not read line (" << linecount << ") from file \"" << filepath << "\"." << endl;
			std::logic_error(err.str());
		}

		// Reuse stream from vertex count header for vertex list.
		vertex_line_stream.clear();
		vertex_line_stream.str(line);

		vertex_line_stream >> vertices[i].x >> vertices[i].y >>vertices[i].z;

		// Perform homogeneous transform on vertex coordinates.
		float scale_factor = transform.get_scale_factor();
		if (scale_factor != 1.0f) vertices[i] *= scale_factor;
		transform.apply_homogeneous(vertices[i]);

		if (i % 200 == 0) display_progress(vertex_count, i + 1, vertex_counter_txt, vertex_progress_label);
	}

	display_progress(vertex_count, vertex_count, vertex_counter_txt, (char *)"Bounding Mesh...   ");

	bounding_volume = new BoundingVolume(this);

	display_progress(vertex_count, vertex_count, vertex_counter_txt, (char *)"done               ");
	cerr << endl;

	// Read faces (for teapot - triangles).
	int n_vertices;
	char *face_counter_txt = (char *)"faces";
	char *face_progress_label = (char *)"Reading faces...";

	triangles = new TriangleIdxMap[face_count];
	face_normals = new Vector[face_count];
	vertex_normals = new Vector[vertex_count];

	display_progress(face_count, 0, face_counter_txt, face_progress_label);
	for (int j = 0; j < face_count; j++)
	{
		try {
			getline(meshfile, line);
			linecount++;
		} catch(ifstream::failure e) {
			std::ostringstream err;
			err << endl << "ERR: Could not read line (" << linecount << ") from file \"" << filepath << "\"." << endl;
			std::logic_error(err.str());
		}

		// Reuse stream from face count header for face list.
		face_line_stream.clear();
		face_line_stream.str(line);

		// Index each face's component vertices.
		face_line_stream >> n_vertices >> triangles[j][0] >> triangles[j][1] >> triangles[j][2];

		if (n_vertices != 3) {
			std::ostringstream err;
			err << endl << "ERR: Non-triangle meshes are not supported. In file \"" << filepath << "\", line (" << linecount << ")." << endl;
			std::logic_error(err.str());
		}

		face_normals[j] = compute_face_normal(j);

		if (j % 200 == 0) display_progress(face_count, 0, face_counter_txt, face_progress_label);
	}

	display_progress(face_count, face_count, face_counter_txt, (char *)"Calculating normals");

	compute_vertex_normals();
	meshfile.close();

	display_progress(face_count, face_count, face_counter_txt, (char *)"done               ");
	cerr << endl << endl;
}

Vector PolyMesh::compute_face_normal(int triangle_idx) {
	// N = (B - A) x (C - A) where A, B, C are vertices of the triangle. (i.e cross of two sides)

	Vector b_a = vertices[triangles[triangle_idx][1]] - vertices[triangles[triangle_idx][0]];
	Vector c_a = vertices[triangles[triangle_idx][2]] - vertices[triangles[triangle_idx][0]];

	return Vector::norm(-Vector::cross_product(b_a, c_a));
}

void PolyMesh::compute_vertex_normals() {
	// Average the normals of the three adjacent faces per vertex.
	for (int i = 0; i < face_count; i++) {
		for (int j = 0; j < 3; j++) {
			vertex_normals[triangles[i][j]] += face_normals[i];
		}
	}

	// Ensure normalised
	for (int k = 0; k < vertex_count; k++) {
		vertex_normals[k].normalise();
	}
}

/// @brief Perform moller-trumbore intersection
/// @param ray Intersection ray to cast.
/// @param v0 Side a of triangle face.
/// @param v1 Side b of triange face.
/// @param v2 Side c of triangle face.
/// @param u Barycentric coord u.
/// @param v Barycentric coord v.
/// @param t Coefficient of ray direction indicating distance at which intersection occurred.
/// @return If intersection occured.
bool PolyMesh::moller_trumbore(
	const Ray& ray, 
	const Vector &v0, const Vector &v1, const Vector &v2, 
	float &t, float &u, float &v
) {
	
	const float epsilon = 0.00001f;
	Vector edge1, edge2, h, s, q;
	float a, f;

	edge1 = v1 - v0;
	edge2 = v2 - v0;

	h = Vector::cross_product(ray.direction, edge2);
	a = Vector::dot_product(edge1, h);

	if (a > -epsilon && a < epsilon) return false;

	f = 1.0f/a;
	s = ray.position - v0;
	u = f * Vector::dot_product(s, h);

	if (u < 0.0f || u > 1.0f) return false; // Ensure valid barycentric coordinates (i.e. not outside triangle).

	q = Vector::cross_product(s, edge1);
	v = f * Vector::dot_product(ray.direction, q);

	if (v < 0.0f || u + v > 1.0f) return false; // Barycentric coord check again.

	t = f * Vector::dot_product(edge2, q);

	if (t > epsilon) return true;
	return false; // Ignore intersections behind the ray origin.
}

bool PolyMesh::intersect_triangle(Ray ray, Hit &hit, int triangle_idx) {

	// Tolerance on intersection acceptance - helps prevent 'accidentally' rendering a back-face at edges.
	const float epsilon = 0.00000001f;

	// Ray is parallel to face normal (within epsilon tolerance) so don't intersect.
	if (fabs(face_normals[triangle_idx].dot(ray.direction)) < epsilon) return false;

	// Get vertices of triangle specified.
	Vector v0 = vertices[triangles[triangle_idx][0]];
	Vector v1 = vertices[triangles[triangle_idx][1]];
	Vector v2 = vertices[triangles[triangle_idx][2]];

	float t, u, v;
	bool intersection = moller_trumbore(ray, v0, v1, v2, t, u, v) ;

	if (!intersection) return false;

	hit.t = t;
	hit.obj = this;
	hit.position = ray.position + t * ray.direction;

	if (interpolate_normals) {
		// Lerp between vertex normals using triangle's barycentric coords.
		hit.normal = 
			vertex_normals[triangles[triangle_idx][0]] * (1 - u - v) +
			vertex_normals[triangles[triangle_idx][1]] * u + 
			vertex_normals[triangles[triangle_idx][2]] * v;
		hit.normal.normalise();
	} else {
		// Otherwise just use the face normal.
		hit.normal = face_normals[triangle_idx];
		hit.normal.normalise();
	}

	return true;
}

void PolyMesh::compute_bounds(Vector plane_norm, float &near, float &far) {
	float d;
	for (int i = 0; i < vertex_count; i++) {
		d = Vector::dot_product(plane_norm, vertices[i]);
		if (d < near) near = d;
		if (d > far) far = d;
	}
}

vector<Hit> PolyMesh::intersection(Ray ray) {

	vector<Hit> hits;

	if (!bounding_volume->intersect(ray)) return hits;

	bool intersection;
	
	// TODO - Potential fix to self-shadow casting present on gourand-shaded model can be found at:
	// https://computergraphics.stackexchange.com/questions/4986/ray-tracing-shadows-the-shadow-line-artifact
	bool entering = true;
	for(int i = 0; i < face_count; i++) {
		// Check all triangles for intersections
		Hit triangle_hit; // TODO - this may be slow.
		intersection = intersect_triangle(ray, triangle_hit, i);

		if (intersection) {
			triangle_hit.entering = entering;
			entering = !entering;
			hits.emplace_back(triangle_hit);
		}
	}
	
	// Ensure hits are in correct order along ray cast - there are faster ways of doing this.
	std::sort(hits.begin(), hits.end(), [](const Hit &l, const Hit &r){
		return l.t < r.t;
	});

	return hits;

}