//
// Created by sysgen on 7/2/22.
//

#ifndef PATHTRACER_SYCL_SCENE_H
#define PATHTRACER_SYCL_SCENE_H


#include <vector>
#include "Sphere.h"
#include "kdTreeMesh.h"

class scene {
public:
    void makeScene(std::vector<kdTreeMesh> &kVec, std::vector<Sphere> &sVec, sc::queue &q);
};


#endif //PATHTRACER_SYCL_SCENE_H
