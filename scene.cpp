//
// Created by sysgen on 7/2/22.
//

#include "scene.h"
namespace sc = cl::sycl;

void scene::makeScene(std::vector<kdTreeMesh> &kVec, std::vector<Sphere> &sVec, sc::queue &q) {
    AABB bounds = AABB();
    auto tris = readMesh(bounds, "dragon-low.obj", true);
//    moveResize(tris, bounds, {0, 0, -0.05}, {.3, .3, .3});
    materialBase mat = materialBase({1.2, .5, .5}, 1.4, MATERIALS::Glass); // Not physically correct but it looks cool
    mat = materialBase({0.9, .7, .0}, .2, MATERIALS::Glossy); // Not physically correct but it looks cool
    kdTreeMesh k = kdTreeMesh(tris, bounds, 18, 40, q, mat);
//    k.parentNode.to_device(q);
    kVec.push_back(k);
//    mat = materialBase({.6, .6, .6}, .0, MATERIALS::Diffuse);
    mat = materialBase({.6, .6, .6}, .5, MATERIALS::Glossy);
    tris = readMesh(bounds, "plane.obj", true);
    kVec.emplace_back(kdTreeMesh(tris, bounds, 5, 20, q, mat));
    sVec.push_back(Sphere({-0, -0, -4}, 1, sc::float3({255, 255, 255}) * 40, MATERIALS::Emissive, 1));
//    sVec.push_back(Sphere({0, 3, -1.2}, 1.5, sc::float3({.8, .5, .5}), MATERIALS::Glossy, 0));
}
