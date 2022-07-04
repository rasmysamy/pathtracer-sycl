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
    inline Ray getRay(float x, float y) const {
        sc::float3 horizontalComp = (horizontalVector*(x-(X/2.0f))*virtualRecScale);
        sc::float3 verticalComp = (upVector*(y-(Y/2.0f))*virtualRecScale);
        sc::float3 forwardComp = principalRay.getDirection()*virtualRecDistance;
        return Ray(principalRay.getOrigin(),
                   sc::normalize(horizontalComp+verticalComp+forwardComp));
    }

    inline std::vector<std::vector<Ray>> getRays() const {
        std::vector<std::vector<Ray>> rays;
        rays.resize(X, std::vector<Ray>(Y));
        for (int i = 0; i < X; i++){
            for(int j = 0; j < Y; j++){
                rays[i][j] = getRay(i,j);
            }
        }
        return rays;
    }


};


#endif //UNTITLED1_CAMERA_H
