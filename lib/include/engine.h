#ifndef _RENDERENGINE_
#define _RENDERENGINE_

#include "config/config.h"

#include "data/bounding_box.h"
#include "data/bounding_volume.h"
#include "data/colour.h"
#include "data/framebuffer.h"
#include "data/hit.h"
#include "data/kd_tree.h"
#include "data/object.h"
#include "data/photon.h"
#include "data/ray.h"
#include "data/transform.h"
#include "data/vector.h"

#include "geometry/csg.h"
#include "geometry/cube.h"
#include "geometry/object3D.h"
#include "geometry/plane.h"
#include "geometry/polymesh.h"
#include "geometry/quadric.h"
#include "geometry/sphere.h"

#include "lights/light.h"
#include "lights/point_light.h"
#include "lights/square_light.h"

#include "materials/dielectric.h"
#include "materials/emissive.h"
#include "materials/global_material.h"
#include "materials/lambertian.h"
#include "materials/material.h"
#include "materials/mirror.h"
#include "materials/normat.h"
#include "materials/phong.h"

#include "rendering/camera.h"
#include "rendering/scene.h"

#include "textures/block_texture.h"
#include "textures/checker_texture.h"
#include "textures/image_texture.h"
#include "textures/texture.h"

#include "util/util.h"

#endif