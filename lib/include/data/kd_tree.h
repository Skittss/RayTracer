#pragma once

#include "photon.h"
#include "../util/util.h"

#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <float.h>

struct kd_node {
    Photon photon;
    kd_node *left = nullptr;
    kd_node *right = nullptr;
};

struct kd_dist {
    Photon photon;
    float dist;
};

struct kd_dist_cmp {
    bool operator()(const kd_dist &lhs, const kd_dist &rhs) {
        return lhs.dist < rhs.dist;
    }
};

struct bake_param_cont {
    unsigned begin;
    unsigned end;
    kd_node *parent;
    unsigned child_side;
    unsigned d;
};

class KdTree {
private:

    vector<kd_node*> node_lst; // TODO: Clear this vector after baking if memory becomes short.
    kd_node *root = nullptr;
    int size = 0;

    std::priority_queue<kd_dist, vector<kd_dist>, kd_dist_cmp> k_best_q;
    int curr_best_k_query = 1;

    bool tree_baked = false;

    kd_node* new_node(Photon p) {
        kd_node *n = new kd_node;
        n->photon = p;
        return n;
    }

    kd_dist new_dist_cont(Photon p, float d) {
        kd_dist c;
        c.photon = p;
        c.dist = d;
        return c;
    }

    bake_param_cont* new_bake_param_cont(unsigned begin, unsigned end, kd_node *parent, unsigned child_side, unsigned d) {
        bake_param_cont *c = new bake_param_cont;
        c->begin = begin;
        c->end = end;
        c->parent = parent;
        c->child_side = child_side;
        c->d = d;

        return c;
    }

    /**
     * Adds a photon to the photon map. 
     * Done iteratively to avoid stack overflow from recursion when the map is very large.
     * Note that calling this function many times may destroy the balance of the tree.
     * 
     * @param p the photon to add to the map.
     * @return pointer to the new `root`.
     */
    kd_node* add_elmt_iter(Photon p) {

        if (root == nullptr) return new_node(p);

        kd_node *node = root;
        unsigned d = 0;

        bool done = false;
        while (!done) {
            unsigned current_dim = d % 3;

            if (p.pos[current_dim] < node->photon.pos[current_dim]) {
                if (node->left == nullptr) {node->left = new_node(p); done = true;}
                else node = node->left;
            } else {
                if (node->right == nullptr) {node->right = new_node(p); done = true;}
                else node = node->right;
            }

            d++;
        }
        return root;

    }

    void k_nearest(kd_node *node, Vector &p, unsigned d) {
    
        if (node == nullptr) return;

        // Shift dimension in kd-tree to check.
        unsigned current_dim = d % 3;

        float x, y, z;
        Vector pos = node->photon.pos;
        x=pos.x-p.x; y=pos.y-p.y; z=pos.z-p.z;
        float dist = x * x + y * y + z * z;
        // float dist = (node->photon.ray.position - p).length_squared();
        if (k_best_q.size() < curr_best_k_query) {
            // Update best dist if closer.
            k_best_q.push(new_dist_cont(node->photon, dist));
        } else if (dist < k_best_q.top().dist) {
            // Update best k if closer than worst nearest neighbour
            k_best_q.pop(); // Remove worst
            k_best_q.push(new_dist_cont(node->photon, dist));
        }

        float dim_dist = node->photon.pos[current_dim] - p[current_dim];
        // Check smallest side (bounding box)
        k_nearest(dim_dist > 0.0f ? node->left : node->right, p, d + 1);
        if (dim_dist * dim_dist >= k_best_q.top().dist) return;
        // Only check other side if potentially closer than best.
        k_nearest(dim_dist > 0.0f ? node->right : node->left, p, d + 1);
    }

public:

    // Adds a photon to the tree **after** it has been baked.
    void post_bake_add(const Photon p) {
        if (!tree_baked) throw std::logic_error("ERR: Tried to post-add node before tree was baked.");
        add_elmt_iter(p);
        size++;
    }

    // Adds a photon to the tree **before** it has been baked.
    void add(const Photon p) {
        if (tree_baked) throw std::logic_error("ERR: Tried to pre-add node after tree was baked.");
        node_lst.push_back(new_node(p));
        size++;
    }

    int get_size() {
        return size;
    }

    //Internally constructs a balanced kd-tree from the back-list of photons added to the map. 
    void bake_tree() {

        stack<bake_param_cont*> call_stack;
        
        call_stack.push(new_bake_param_cont(0, node_lst.size(), nullptr, 0, 0));

        auto i = node_lst.begin(); // Vector iteration address offset
        unsigned begin;
        unsigned end;
        kd_node *parent;
        unsigned child_side;
        unsigned d;

        while (call_stack.size() > 0) {

            bake_param_cont *frame = call_stack.top(); call_stack.pop();
            begin      = frame->begin;
            end        = frame->end;
            parent     = frame->parent;
            child_side = frame->child_side;
            d          = frame-> d;
            

            if (end <= begin) continue;

            // Partial sort node lst so that idx n = median, idx < n are smaller than median, idx > n are larger...
            unsigned nth = begin + (end - begin) / 2;
            std::nth_element(
                i + begin, i + nth, i + end,             
                [d](kd_node* a, kd_node* b) {return a->photon.pos[d] < b->photon.pos[d];}
            );

            // Construct tree by applying children medians to parent node.
            if (parent == nullptr) {root = node_lst[nth];}
            else {
                if (child_side == 0) parent->left = node_lst[nth];
                else parent->right = node_lst[nth];
            }

            // Add new split params to 'call stack'.
            
            // Traverse Left
            call_stack.push(new_bake_param_cont(begin, nth, node_lst[nth], 0, (d+1)%3));
            // Traverse Right
            call_stack.push(new_bake_param_cont(nth + 1, end, node_lst[nth], 1, (d+1)%3));
        }

        tree_baked = true;
    }

    void knn(Vector &p, int n) {
        if (!tree_baked) throw std::logic_error("ERR: Tried to perform knn on an unbaked tree.");
        // if (size < n) throw std::logic_error("ERR: Nearest k neighbours could not be found: Tree is too small.");
        
        k_best_q = std::priority_queue<kd_dist, vector<kd_dist>, kd_dist_cmp>();
        curr_best_k_query = n;

        k_nearest(root, p, 0);
    }

    vector<Photon> get_estimate_photons(Vector &p, int n, float &r2) {
        knn(p, n);

        int size = k_best_q.size();
        vector<Photon> arr(size);
        for (int i = 0; i < size; i++) {
            if (i == 0) r2 = k_best_q.top().dist;
            arr[i] = k_best_q.top().photon;
            k_best_q.pop();
        }

        return arr;
    }

    vector<Photon> get_estimate_photons_bounded(Vector &p, int n, float r2) {
        knn(p, n);
        int size = k_best_q.size();
        vector<Photon> arr;
        for (int i = 0; i < size; i++) {
            if (k_best_q.top().dist <= r2) arr.emplace_back(k_best_q.top().photon);
            k_best_q.pop(); 
        }

        return arr;
    }


    void test_knn() {
        int n_pts = 100000;
        Vector query = Vector(UTIL::rand_float(-100, 100), UTIL::rand_float(-100, 100), UTIL::rand_float(-100, 100));
        cerr << "Query pt: " << query.x << "," << query.y << "," << query.z << endl;

        priority_queue<float> best_dists;
        float n = 100;
        
        Vector pts[n_pts];
        // Vector *best_p = nullptr; float best_dist = 0;
        for (int i = 0; i < n_pts; i++) {
            pts[i] = Vector(
                UTIL::rand_float(-100, 100),
                UTIL::rand_float(-100, 100),
                UTIL::rand_float(-100, 100)
            );
            if (best_dists.size() < n) {
                best_dists.push((query - pts[i]).length_squared());
            } else if ((query - pts[i]).length_squared() < best_dists.top()) {
                best_dists.push((query - pts[i]).length_squared());
                best_dists.pop();
            }
        }

        cerr << "Closest from naive:" << endl;
        for (int i = 0; i < n; i++) {
            float dst = best_dists.top(); best_dists.pop();
            cerr << i << ": " << dst << endl;
        }
        cerr << endl;

        for (int j = 0; j < n_pts; j++) {
            add(Photon(pts[j], Vector(0.0f, 0.0f, 0.0f), Colour(0.0f)));
        }
        bake_tree();

        knn(query, n);

        for (int i = 0; i < n; i++) {
            Vector pos = k_best_q.top().photon.pos;
            float dst = k_best_q.top().dist;
            cerr << i << ": " << pos.x << "," << pos.y << "," << pos.z << " | " << dst << endl;
            k_best_q.pop();
        }

        float r2;
        vector<Photon> close = get_estimate_photons(query, n, r2);

        cerr << endl << "From Gather func - Max Dist Sq: " << r2 << endl;
    }
};