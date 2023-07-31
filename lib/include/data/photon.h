#pragma once

#include "ray.h"
#include "vector.h"
#include "colour.h"

typedef struct {
    float x, y, z;
    char pow[4];
    char phi, theta;
    short flag;
} c_photon;

class Photon {
public:
    Colour flux;
    Vector pos;
    Vector wi; // Incident radiance dir

	Photon() {}

	Photon(Vector pos, Vector wo, Colour pow) : pos(pos), flux(pow) {}

    Photon(Vector pos, Vector wo, Vector wi, Colour pow) : 
        pos(pos),
        flux(pow),
        wi(wi) 
    {}
};