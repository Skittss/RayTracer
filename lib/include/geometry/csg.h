#pragma once

#include "object3D.h"
#include "sphere.h" // For testing

#include <set>

// TODO: A better way of managing csg would be for the cdg object to hold information about the rotation and position
//         of objects, and instantiate them itself. Currently objects must be created with the right rotation & position
//         and managing this for each object can be somewhat tricky, instead of via a global transform.

// TODO: A BV / BVH would be a useful optimisation here for larger CSG trees.

enum class CSGType {UNION, DIFFERENCE, INTERSECTION, NONE};

class CSG : public Object3D {

public:
    struct csg_tree_node {
        CSGType operation;
        Object3D *obj; // Only used in leaf nodes;

        csg_tree_node *left = nullptr;
        csg_tree_node *right = nullptr;
    };

    csg_tree_node *csg_tree_root;

    CSG() {
        material = nullptr;
    }

    vector<Hit> intersection(Ray ray) {
        // Post order traversal of tree
        return post_order_intersect(ray, csg_tree_root);
    }
        
    /// @brief Compose two arbitrary objects via csg.
    /// @param operation UNION, INTERSECTION, DIFFERENCE, or NONE.
    /// @return CSG binary tree node.
    static csg_tree_node* compose_obj_pair(csg_tree_node *a, csg_tree_node *b, CSGType operation) {
        return compose_tree_nodes(a, b, operation);
    }

    static csg_tree_node* compose_obj_pair(Object3D *a, Object3D *b, CSGType operation) {
        csg_tree_node *node_a = create_leaf_node(a);
        csg_tree_node *node_b = create_leaf_node(b);
        
        return compose_tree_nodes(node_a, node_b, operation);
    }
    
    static csg_tree_node* compose_obj_pair(csg_tree_node *a, Object3D *b, CSGType operation) {
        csg_tree_node *node_b = create_leaf_node(b);

        return compose_tree_nodes(a, node_b, operation);
    }

    static csg_tree_node* compose_obj_pair(Object3D *a, csg_tree_node *b, CSGType operation) {
        csg_tree_node *node_a = create_leaf_node(a);

        return compose_tree_nodes(node_a, b, operation);
    }

    /// @brief Test Union, Intersection and Difference of two one-dimensional number-lines of t values.
    void test_csg() {
        Ray ray(0.0f, Vector(1.0f));

        Sphere *gs = new Sphere(Vector(0.0f), 0.5f);
        Sphere *rs = new Sphere(Vector(0.0f), 1.0f);

        Hit g1, g2, g3, g4, g5, g6, g7, g8, r1, r2, r3, r4, r5, r6;
        g1.t = 0.1f; g1.normal = -ray.direction; g1.obj = gs;
        r1.t = 0.2f; r1.normal = -ray.direction; r1.obj = rs;
        g2.t = 0.3f; g2.normal = ray.direction; g2.obj = gs;
        g3.t = 0.4f; g3.normal = -ray.direction; g3.obj = gs;
        r2.t = 0.5f; r2.normal = ray.direction; r2.obj = rs;
        r3.t = 0.6f; r3.normal = -ray.direction; r3.obj = rs;
        g4.t = 0.7f; g4.normal = ray.direction; g4.obj = gs;
        g5.t = 0.8f; g5.normal = -ray.direction; g5.obj = gs;
        g6.t = 0.9f; g6.normal = ray.direction; g6.obj = gs;
        r4.t = 1.0f; r4.normal = ray.direction; r4.obj = rs;
        r5.t = 1.1f; r5.normal = -ray.direction; r5.obj = rs;
        g7.t = 1.2f; g7.normal = -ray.direction; g7.obj = gs;
        r6.t = 1.3f; r6.normal = ray.direction; r6.obj = rs;
        g8.t = 1.4f; g8.normal = ray.direction; g8.obj = gs;

        set<Object3D*> g_obj {gs};
        set<Object3D*> r_obj {rs};

        vector<Hit> g {g1, g2, g3, g4, g5, g6, g7, g8};
        vector<Hit> r {r1, r2, r3, r4, r5, r6};

        for (Hit h : g) h.obj = gs;
        for (Hit h : r) h.obj = rs;


        vector<Hit> isect = csg_intersection(ray, g, r, g_obj); 

        cerr << "isect" << endl;
        for (Hit h : isect) cerr << h.t << " ";
        cerr << endl << endl;

        vector<Hit> diff = csg_difference(ray, r, g, r_obj); 

        cerr << "diff" << endl;
        for (Hit h : diff) cerr << h.t << " ";
        cerr << endl << endl;

        vector<Hit> un = csg_union(ray, g, r); 
        cerr << "union" << endl;
        for (Hit h : un) cerr << h.t << " ";
        cerr << endl;
    }

private:

    static csg_tree_node* create_leaf_node(Object3D *obj) {
        csg_tree_node *node = new csg_tree_node();
        node->operation = CSGType::NONE;
        node->obj = obj;

        return node;
    }

    /// @brief combine two tree nodes via a csg operation.
    static csg_tree_node* compose_tree_nodes(csg_tree_node *a, csg_tree_node *b, CSGType operation) {
        csg_tree_node *compose = new csg_tree_node();
        compose->operation = operation;
        compose->left = a;
        compose->right = b;

        return compose;
    }

    /// @brief get composed Object3D's from the supplied subtree.
    static void get_subtree_primitives(set<Object3D*> &primitives, csg_tree_node *node) {
        if (node->operation == CSGType::NONE) primitives.insert(node->obj);
        else {
            get_subtree_primitives(primitives, node->left);
            get_subtree_primitives(primitives, node->right);
        }
    }

    /// @brief Post order traversal of the csg tree to calculate intersections from 
    /// composed csg operations from the tree bottom-up.
    /// @param ray Ray to cast.
    /// @param node Current traversal node (pass in root to do full traversal)
    /// @return 
    vector<Hit> post_order_intersect(const Ray &ray, csg_tree_node *node) {
        if (node->operation == CSGType::NONE) {
            return node->obj->intersection(ray);
        }

        vector<Hit> span_l = post_order_intersect(ray, node->left);
        vector<Hit> span_r = post_order_intersect(ray, node->right);

        if (node->operation == CSGType::UNION) {
            return csg_union(ray, span_l, span_r);
        } else if (node->operation == CSGType::INTERSECTION) {
            set<Object3D*> l_primitives;
            get_subtree_primitives(l_primitives, node->left); // TODO: this could be memoized.
            return csg_intersection(ray, span_l, span_r, l_primitives);
        } else { // Difference
            set<Object3D*> l_primitives;
            get_subtree_primitives(l_primitives, node->left); // TODO: this could be memoized.
            return csg_difference(ray, span_l, span_r, l_primitives);
        }
    }

    vector<Hit> csg_union(const Ray &ray, const vector<Hit> &hits_l, const vector<Hit> &hits_r) {
        vector<Hit> hits = merge_span_hits(hits_l, hits_r);
        vector<Hit> union_hits;

        if (hits.size() < 2) return union_hits;
        
        // Similar method to matching pairs of brackets - keep count of entry hit 'depth'.
        int cnt = 0;
        for (int i = 0; i < hits.size(); i++) {
            if (cnt == 0 && hits[i].entering) union_hits.emplace_back(hits[i]);

            if (hits[i].entering) cnt++;
            else cnt--;

            if (cnt == 0) union_hits.emplace_back(hits[i]);
        }

        return union_hits;
    }

    /// @brief Intersection of constructive solid geometry.
    vector<Hit> csg_intersection(
        const Ray &ray, 
        const vector<Hit> &hits_l,
        const vector<Hit> &hits_r,
        const set<Object3D*> &objs_l
    ) {

        vector<Hit> intersection;

        // No intersection if one obj is not hit at all, and planes are degenerative case.
        if (hits_l.size() < 2 || hits_r.size() < 2) return intersection; 

        bool was_in_l = !hits_l[0].entering; 
        bool was_in_r = !hits_r[0].entering; // Assume convex solids

        vector<Hit> all_hits = merge_span_hits(hits_l, hits_r);

        for (Hit hit : all_hits) {
            bool going_in = hit.entering;
            bool is_l_obj = (objs_l.find(hit.obj) != objs_l.end());

            bool is_in_l = was_in_l;
            bool is_in_r = was_in_r;

            if (is_l_obj) is_in_l = going_in;
            else is_in_r = going_in;

            if (((was_in_r != is_in_r) && (was_in_l || is_in_l)) ||
                ((was_in_l != is_in_l) && (was_in_r || is_in_r))) 
            {
                intersection.emplace_back(hit);
            }

            was_in_l = is_in_l;
            was_in_r = is_in_r;
        }
        
        return intersection;
    }

    /// @brief Difference of constructive solid geometry.
    vector<Hit> csg_difference(
        const Ray &ray, 
        const vector<Hit> &hits_l, 
        const vector<Hit> &hits_r,
        const set<Object3D*> &objs_l
    ) {

        if (hits_l.size() == 0) return hits_l; // No difference if one obj is not hit at all.
        if (hits_r.size() == 0) return hits_l;

        vector<Hit> difference;

        bool was_in_l = !hits_l[0].entering; 
        bool was_in_r = !hits_r[0].entering; // Assume convex solids

        vector<Hit> all_hits = merge_span_hits(hits_l, hits_r);

        for (Hit hit : all_hits) {
            bool going_in = hit.entering;
            bool is_l_obj = objs_l.find(hit.obj) != objs_l.end();

            bool is_in_l = was_in_l;
            bool is_in_r = was_in_r;

            if (is_l_obj) is_in_l = going_in;
            else is_in_r = going_in;

            if (((was_in_l != is_in_l) && ((!is_in_r) && (!was_in_r))) ||
                ((was_in_r != is_in_r) && (was_in_l || is_in_l)))
            {
                if (!is_l_obj) hit.normal.negate();
                difference.emplace_back(hit);
            }

            was_in_l = is_in_l;
            was_in_r = is_in_r;
        }

        return difference;
    }

    vector<Hit> merge_span_hits(const vector<Hit> &hits_l, const vector<Hit> &hits_r) {
        if (hits_l.size() == 0) return hits_r;
        if (hits_r.size() == 0) return hits_l;

        // Merge sort.
        vector<Hit> union_hits(hits_l.size() + hits_r.size());
        std::merge(
            hits_l.begin(), hits_l.end(), hits_r.begin(), hits_r.end(), union_hits.begin(),
            [](const Hit& l, const Hit& r) {return l.t < r.t;}
        );
        return union_hits;
    }

};