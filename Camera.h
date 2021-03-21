//
// Created by sysgen on 2/12/21.
//

#ifndef UNTITLED1_CAMERA_H
#define UNTITLED1_CAMERA_H

#include <CL/sycl.hpp>
#include "Ray.h"

namespace sc = cl::sycl;

class Camera {
private:
    Ray principalRay;
    sc::float3 upVector;
    sc::float3 horizontalVector;
    float rotation;//Currently disabled
    float horizontalFov;
    int X;
    int Y;
    float virtualRecDistance;
    float virtualRecScale;
public:
    Camera(const sc::float3& position, const sc::float3& direction, float hF, float r, int x, int y ):
        principalRay(Ray(position, sc::normalize(direction))), horizontalFov(sc::radians(hF/2)), rotation(r), X(x), Y(y) {
        virtualRecDistance = 1000;
        virtualRecScale = sc::tan(horizontalFov); // NOLINT(cppcoreguidelines-narrowing-conversions)
        virtualRecScale = 1000*virtualRecScale/(X);
        upVector = {0,0,1};
        horizontalVector = sc::normalize(sc::cross(direction, upVector));
        upVector = sc::normalize(sc::cross(horizontalVector, direction)); //This may lead to an inverted camera, verify
    };
    Ray getRay(int x, int y) const;
    std::vector<std::vector<Ray>> getRays() const;

};


#endif //UNTITLED1_CAMERA_H
