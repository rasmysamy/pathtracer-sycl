//
// Created by sysgen on 2/11/21.
//

#ifndef UNTITLED1_RAY_H
#define UNTITLED1_RAY_H

#include <CL/sycl.hpp>
#include <iostream>

namespace sc = cl::sycl;

class Ray {
private:
    sc::float3 origin;
    sc::float3 direction;
    sc::float3 luminance;
public:
    Ray(const sc::float3& o, const sc::float3& d):origin(o), direction(d),luminance({1,1,1}){}
    Ray():origin({0,0,0}), direction({0,0,0}){}
    const sycl::float3 &getOrigin() const {return origin;}
    void setOrigin(const sycl::float3 &origin) {Ray::origin = origin;}
    const sycl::float3 &getDirection() const {return direction;}
    void setDirection(const sycl::float3 &direction) {Ray::direction = direction;}
    const sycl::float3 &getLuminance() const {return luminance;}
    void setLuminance(const sycl::float3 &luminance) {Ray::luminance = luminance;}

    void operator -(){direction = -direction;}
    friend std::ostream& operator <<(std::ostream& os, const Ray & r){
        os << "Origin: " << vecToString(r.origin)
            << " Direction: " << vecToString(r.direction)
            << " Luminance(RGB) " << vecToString(r.luminance);
        return os;
    }

    sc::float3 closestPointOnRay(const sycl::float3 &p) const{
        sc::float3 closestPointOnLine = origin + ((sc::normalize(direction) * sc::dot(direction, (p-origin))));
        if(sc::dot(closestPointOnLine-origin, direction) > 0)
            return closestPointOnLine;
        return origin;
    }

    float pointDistanceFrom(const sycl::float3 &p) const{
        return sc::length(p - closestPointOnRay(p));
    }

    bool beforeStart(const sycl::float3 &p) const {
        return sc::dot(p, direction) <= sc::dot(origin, direction);
    }

    static std::string vecToString(const sc::float3 &vec){
        return std::string("{") + std::to_string(vec.x()) + ", "
               + std::to_string(vec.y()) + ", "
               + std::to_string(vec.z()) + "}";
    }

};

#endif //UNTITLED1_RAY_H
