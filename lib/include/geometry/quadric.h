#pragma once

#include "../geometry/object3D.h"
#include "../data/bounding_box.h"

enum class UNIT_QUADRIC {
    ELLIPSE, CYLINDER, HYPERBOLIC_CYLINDER, CONE, PARABOLOID, 
    HYPERBOLIC_PARABOLOID, HYPERBOLOID_ONE_SHEET, HYPERBOLOID_TWO_SHEET
};

class Quadric : public Object3D {

private:
    BoundingBox *aabb;

public:
    //ax^2 + by^2 + cz^2 + dxy + exz + fyz + gx + hy + iz + j = 0
    float a, b, c, d, e, f, g, h, i, j;
    bool twoside = false;

    Quadric(
        UNIT_QUADRIC type, Vector position, float size, 
        float bbox_size = -1.0f,
        float c_a = 1.0f, float c_b = 1.0f, float c_c = 1.0f
    ) {
        // Default orientation is along y-axis
        this->position = position;
        if (bbox_size < 0) bbox_size = size;
        float eps_size = bbox_size + 0.0001f;
        this->aabb = new BoundingBox(position, eps_size, eps_size, eps_size);
        j = -size;
        switch (type) {
        case UNIT_QUADRIC::ELLIPSE:
            a=c_a; b=c_b; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; 
            break;
        case UNIT_QUADRIC::CYLINDER:
            a=c_a; b=0.0f; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; 
            break;
        case UNIT_QUADRIC::HYPERBOLIC_CYLINDER:
            // Note larger bounding box than size bounds required as hyperbolic curve has asymtote at size.
            a=c_a; b=0.0f; c=-c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; 
            break;
        case UNIT_QUADRIC::CONE:
            a=c_a; b=-c_b; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f, j=0.0f; 
            break;
        case UNIT_QUADRIC::PARABOLOID:
            a=c_a; b=0.0f; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=-1.0f; i=0.0f; j=0.0f;
            break;
        case UNIT_QUADRIC::HYPERBOLIC_PARABOLOID:
            a=c_a; b=0.0f; c=-c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=-1.0f; i=0.0f; j=0.0f;
            break;
        case UNIT_QUADRIC::HYPERBOLOID_ONE_SHEET:
            a=c_a; b=-c_b; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; 
            break;
        case UNIT_QUADRIC::HYPERBOLOID_TWO_SHEET:
            // Larger bounding box required for same reason as hyperbolic cylinder.
            a=c_a; b=-c_b; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; j=-j;
            break;
        default:
            // Sphere is default.
            a=c_a; b=c_b; c=c_c; d=0.0f; e=0.0f; f=0.0f; g=0.0f; h=0.0f; i=0.0f; 
            break;
        }
    }
    
    // For arbitrary quadric surface
    Quadric(
        Vector position, 
        float a, float b, float c, float d, float e, float f, float g, float h, float i, float j
    ) : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j) {
        this->position = position;
    }

    void get_uv(Vector pos, float &u, float &v) {
        u = 0;
        v = 0;
    }

    // See 
    //  http://skuld.bmsc.washington.edu/people/merritt/graphics/quadrics.html
    // For further explanation of this algorithm
    vector<Hit> intersection(Ray ray) override {

        vector<Hit> hits;

        if (!aabb->intersect(ray)) return hits;

        Vector D = ray.direction;
        Vector O = ray.position - position;

        // TODO: this can be turned into matrix calculation & dot products.
        float aq = a*D.x*D.x + b*D.y*D.y + c*D.z*D.z + d*D.x*D.y + e*D.x*D.z + f*D.y*D.z; 
        float bq = 2*a*O.x*D.x + 2*b*O.y*D.y + 2*c*O.z*D.z + d*(O.x*D.y + O.y*D.x) + e*(O.x*D.z + O.z*D.x) + f*(O.y*D.z + D.y*O.z) + g*D.x + h*D.y + i*D.z;
        float cq = a*O.x*O.x + b*O.y*O.y + c*O.z*O.z + d*O.x*O.y + e*O.x*O.z + f*O.y*O.z + g*O.x + h*O.y + i*O.z + j;

        float discrim = bq * bq - 4 * aq * cq;

        if (discrim < 0.0f) return hits;

        float sqrt_d = sqrtf(discrim);

        // Check smallest root first
        float t = (-bq - sqrt_d) / (2*aq);
        Vector vi;

        Hit small_root;
        small_root.obj = this; 						
        small_root.t = t; 
        vi = (ray.position + small_root.t * ray.direction) - position; // Hit pos in local space.
        small_root.position = vi + position;
        small_root.normal = Vector::norm(Vector(
            2*a*vi.x + d*vi.y + e*vi.z + g,
            2*b*vi.y + d*vi.x + f*vi.z + h,
            2*c*vi.z + e*vi.x + f*vi.y + i
        ));
        if (twoside && small_root.normal.dot(ray.direction) > 0) small_root.normal.negate();
        get_uv(small_root.normal, small_root.u, small_root.v);
        if (aabb->in_bounds(small_root.position)) hits.emplace_back(small_root);

        // Check larger root
        t = (-bq + sqrt_d) / (2*aq);

        Hit large_root;
        large_root.obj = this;
        large_root.t = t;
        vi = (ray.position + large_root.t * ray.direction) - position;
        large_root.position = vi + position;
        large_root.normal = Vector::norm(Vector(
            2*a*vi.x + d*vi.y + e*vi.z + g,
            2*b*vi.y + d*vi.x + f*vi.z + h,
            2*c*vi.z + e*vi.x + f*vi.y + i
        ));            
        if (twoside && large_root.normal.dot(ray.direction) > 0) large_root.normal.negate();
        get_uv(large_root.normal, large_root.u, large_root.v);
        if (aabb->in_bounds(large_root.position)) hits.emplace_back(large_root);

        return hits;

    }

};