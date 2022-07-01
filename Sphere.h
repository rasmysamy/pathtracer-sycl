//
// Created by sysgen on 2/11/21.
//

#ifndef UNTITLED1_SPHERE_H
#define UNTITLED1_SPHERE_H

#include <CL/sycl.hpp>
#include "Material.h"

namespace sc = cl::sycl;
using material::MATERIALS;

class Sphere {
private:
    sc::float3 center;
    float radius;
    sc::float3 attr_1;
    int Material;
    float attr_2;

public:
    Sphere(const sc::float3& c, float r):center(c), radius(r), attr_1({0.9,0.9,0.9}), Material(MATERIALS::Diffuse){}
    Sphere(const sc::float3& c, float r, sc::float3 color, int material, float attr_2):center(c), radius(r), attr_1(color), Material(material), attr_2(attr_2){}
//    std::tuple<Ray, bool> rayIntersect(const Ray& r) const{
//        sc::float3 closestPoint = r.closestPointOnRay(center);
//
//        float squareDistanceFromCenter = sc::fast_length(center-closestPoint);
//        squareDistanceFromCenter *= squareDistanceFromCenter;
//        float radiusSquared = radius*radius;
//        float remainderSquared = radiusSquared-squareDistanceFromCenter;
//        sc::float3 direction = r.getDirection();
//        sc::float3 beforePoint = closestPoint - (direction*sc::sqrt(remainderSquared));
//        sc::float3 afterPoint = closestPoint + (direction*sc::sqrt(remainderSquared));
//
//
//
//
//        return r.beforeStart(afterPoint) || squareDistanceFromCenter>radiusSquared ? std::tuple(r, false) :
//               (!r.beforeStart(beforePoint) ? std::tuple(Ray(beforePoint, sc::normalize(beforePoint-center)), true) :
//               std::tuple(Ray(afterPoint, sc::normalize(afterPoint-center)), true));
//    }
//
//    void pRayIntersect(const Ray& r, Ray *retRay, bool *isHit) const{
//        sc::float3 closestPoint = r.closestPointOnRay(center);
//
//        float squareDistanceFromCenter = sc::fast_length(center-closestPoint);
//        squareDistanceFromCenter *= squareDistanceFromCenter;
//        float radiusSquared = radius*radius;
//        float remainderSquared = radiusSquared-squareDistanceFromCenter;
//        *isHit = (remainderSquared > 0);
//        if(!*isHit)
//            return;
//        sc::float3 direction = r.getDirection();
//        sc::float3 afterPoint = closestPoint + (direction*sc::sqrt(remainderSquared));
//        *isHit = !r.beforeStart(afterPoint);
//        if(!*isHit)
//            return;
//        sc::float3 beforePoint = closestPoint - (direction*sc::sqrt(remainderSquared));
//
//        if(!r.beforeStart(beforePoint)) {
//            retRay->setOrigin(afterPoint);
//            retRay->setDirection(afterPoint-center);
//        }
//        else{
//            retRay->setOrigin(beforePoint);
//            retRay->setDirection(beforePoint-center);
//        }
//    }
//
//    material::intersectReturn srayIntersect(const Ray& r) const{
//        sc::float3 closestPoint = r.closestPointOnRay(center);
//
//        float squareDistanceFromCenter = sc::fast_length(center-closestPoint);
//        squareDistanceFromCenter *= squareDistanceFromCenter;
//        float radiusSquared = radius*radius;
//        float remainderSquared = radiusSquared-squareDistanceFromCenter;
//        sc::float3 direction = r.getDirection();
//        sc::float3 beforePoint = closestPoint - (direction*sc::sqrt(remainderSquared));
//        sc::float3 afterPoint = closestPoint + (direction*sc::sqrt(remainderSquared));
//
//        material::intersectReturn ret{};
//
//        ret.hitpoint = r.beforeStart(beforePoint) ? afterPoint : beforePoint;
//        ret.normal = sc::normalize(ret.hitpoint-center);
//        ret.intersect = !(r.beforeStart(afterPoint) || squareDistanceFromCenter>radiusSquared);
////        ret.material = Material;
//        ret.material = 0;
//        ret.attr_1 = attr_1;
//        ret.objType = 0;
//
//        return ret;
//    }
    float fRayIntersect(const Ray& r) const{
        sc::float3 originCenter = r.getOrigin()-center;
        float A = sc::dot(r.getDirection(), r.getDirection());
        float B = sc::dot(originCenter, r.getDirection())*2;
        float C = sc::dot(originCenter, originCenter) - radius*radius;
        float delta = B*B-(4*A*C);
        float deltaSqrt = sc::sqrt(delta);
        float numerator = -B-deltaSqrt;
        return delta > 0 ? (numerator > 0 ? numerator/(2*A) : (numerator+2*deltaSqrt)/(2*A)) : -1;
    }
//    float intersect(const Ray& r) const{
//        sc::float3 L = center - r.getOrigin();
//        float tca = sc::dot(L, r.getDirection());
//        // if (tca < 0) return false;
//        float d2 = sc::dot(L, L) - tca * tca;
//        if (d2 > (radius*radius)) return -1;
//        float thc = sc::sqrt((radius*radius) - d2);
//        float t0 = tca - thc;
//        float t1 = tca + thc;
//
//        if (t0 > t1) std::swap(t0, t1);
//
//        if (t0 < 0) {
//            t0 = t1; // if t0 is negative, let's use t1 instead
//            if (t0 < 0) return -1; // both t0 and t1 are negative
//        }
//
//        float t=t0;
//
//        return t;
//    }
    material::intersectReturn mRayIntersect(const Ray& r, int stackPos) const{
        float dist = fRayIntersect(r);
        material::intersectReturn ret = material::intersectReturn();
        if(dist<0)
            return ret;
        ret.intersectDistance=dist;
        ret.intersect=true;
        ret.objType=0;
        ret.hitpoint=r.getOrigin()+(dist*r.getDirection());
        ret.normal=sc::normalize(ret.hitpoint-center);
        ret.material=Material;
        ret.attr_1=attr_1;
        ret.stackPos=stackPos;
        ret.attr_2=attr_2;
        return ret;
    }
    bool isEmissive() const {return Material==MATERIALS::Emissive;}
    Ray reflect(const Ray& incident, const Ray& normal, sc::float3 rand) const {
        return material::reflectMat(incident, normal, attr_1, Material, rand);
    }
    sc::float3 getPixelEmissive(const Ray& incident) const {return material::getPixelColor(incident, attr_1);}

    const sc::float3 &getCenter() const {
        return center;
    }
//
//    void setCenter(const sc::float3 &center) {
//        Sphere::center = center;
//    }
//
//    float getRadius() const {
//        return radius;
//    }
//
//    void setRadius(float radius) {
//        Sphere::radius = radius;
//    }
};


#endif //UNTITLED1_SPHERE_H
