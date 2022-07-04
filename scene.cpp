//
// Created by sysgen on 7/2/22.
//

#include "scene.h"
namespace sc = cl::sycl;

void scene::makeScene(std::vector<kdTreeMesh> &kVec, std::vector<Sphere> &sVec, sc::queue &q) {
    AABB bounds = AABB();
    auto tris = readMesh(bounds, "dragon-low.obj");
    materialBase mat = materialBase({1.2, .5, .5}, 1.4, MATERIALS::Glass); // Not physically correct but it looks cool
    kVec.emplace_back(kdTreeMesh(tris, bounds, 30, 40, q, mat));
    mat = materialBase({.6, .6, .6}, .0, MATERIALS::Diffuse);
    tris = readMesh(bounds, "plane.obj");
    kVec.emplace_back(kdTreeMesh(tris, bounds, 1, 20, q, mat));
    sVec.push_back(Sphere({-0, -0, -4}, 1, sc::float3({255, 255, 255}) * 40, MATERIALS::Emissive, 1));//Hidden sphere
}
