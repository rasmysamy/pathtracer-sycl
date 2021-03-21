//
// Created by sysgen on 2/11/21.
//

#ifndef UNTITLED1_SPHERE_H
#define UNTITLED1_SPHERE_H

#include <CL/sycl.hpp>
#include "Ray.h"

namespace sc = cl::sycl;

class Sphere {
private:
    sc::float3 center;
    float radius;
public:
    Sphere(const sc::float3& c, float r):center(c), radius(r){}
    std::tuple<Ray, bool> rayIntersect(const Ray& r) const{
        sc::float3 closestPoint = r.closestPointOnRay(center);

        float squareDistanceFromCenter = sc::length(center-closestPoint);
        squareDistanceFromCenter *= squareDistanceFromCenter;
        float radiusSquared = radius*radius;
        float remainderSquared = radiusSquared-squareDistanceFromCenter;

        if(squareDistanceFromCenter>radiusSquared)
            return std::tuple(Ray(), false);

        sc::float3 beforePoint = closestPoint - (r.getDirection()*sc::sqrt(remainderSquared));
        if(!r.beforeStart(beforePoint))
            return std::tuple(Ray(beforePoint, sc::normalize(beforePoint-center)), true);

        sc::float3 afterPoint = closestPoint + (r.getDirection()*sc::sqrt(remainderSquared));
        if(r.beforeStart(afterPoint))
            return std::tuple(Ray(), false);

        return std::tuple(Ray(afterPoint, sc::normalize(afterPoint-center)), true);
    }
};


#endif //UNTITLED1_SPHERE_H
