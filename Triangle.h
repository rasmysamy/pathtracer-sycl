//
// Created by sysgen on 3/24/21.
//

#ifndef PATHTRACER_SYCL_TRIANGLE_H
#define PATHTRACER_SYCL_TRIANGLE_H

#include "Ray.h"
#include "Material.h"

const float EPSILON = 0.00005;

class Triangle {
    sc::float3 attr_1;
    int material;
//    sc::float3 normal = {0,0,0};
//    sc::float3 barycenter = {0,0,0};
//    bool flatShaded = true;
//    sc::float3 vn0, vn1, vn2 = {0,0,0};
public:
    sc::float3 v0, v1, v2;
    Triangle()= default;;
    Triangle(sc::float3 a, sc::float3 b, sc::float3 c): v0(a), v1(b), v2(c), material(material::MATERIALS::Diffuse), attr_1({.9,.9,.9}){}
    Triangle(sc::float3 a, sc::float3 b, sc::float3 c, sc::float3 color, int mat): v0(a), v1(b), v2(c), material(mat), attr_1(color){}

    inline sc::float3 barycentric2Cartesian(const sc::float3& bary) const {
        return bary.x()*v0 + bary.y()*v1 + bary.z()*v2;
    }

    material::intersectReturn rayIntersect(const Ray& r) const {
        float t, u, v;
        // We will use the Moller-Trumbore algorithm for ray intersection
        // We thus use cramer's rule to solve for t, u, v in the following vector equation:
        /*                                  [t]
         *[-r.direction (v1-v0) (v2-v0)]  * |u|  = r.origin - v0
         *                                  [v]                     */
        //We thus use cramer's rule in the scalar triple product form

        material::intersectReturn ret{};


        ret.material = material;
        ret.attr_1 = attr_1;
        ret.objType = 1;
        ret.intersect = false;

        sc::float3 T = r.getOrigin()-v0;
        sc::float3 v0v1 = (v1-v0);
        sc::float3 v0v2 = (v2-v0);
        sc::float3 P = sc::cross(r.getDirection(), v0v2); //We do the cross products only once to save op


        float det = (sc::dot(P, v0v1));

        if(sc::fabs(det)<EPSILON)
            return ret;

        float iDet = 1/det;
//
        u = iDet * sc::dot(P, T);
        if(u>1||u<0)
            return ret;
//
        sc::float3 Q = sc::cross(T, v0v1);
        v = iDet * sc::dot(Q, r.getDirection());
        if(v < 0 || u+v > 1)
            return ret;

        t = iDet * sc::dot(Q, v0v2);
//
        sc::float3 point = r.getOrigin() + t*r.getDirection();
        sc::float3 norm = sc::normalize(sc::cross(v0v1, v0v2));

        //norm = normal; Flat shading, triangle normals
        //norm = vn0*t + vn1*u + vn2*v;

//        return std::tuple(Ray(point, {0,0,0}), !((v>1||v<0) || (u>1||u<0)));

        //retRay->setOrigin(point);
        //retRay->setDirection(norm);

        ret.hitpoint = point;
        ret.intersect = true;
        ret.normal = sc::dot(norm, r.getDirection())<0 ? norm : -norm;

        return ret;
        //return std::tuple(r, false);
    }

    bool isEmissive() const {return material==material::MATERIALS::Emissive;}
    Ray reflect(const Ray& incident, const Ray& normal, sc::float3 rand) const {
        return material::reflectMat(incident, normal, attr_1, material, rand);
    }
    sc::float3 getPixelEmissive(const Ray& incident) const {return material::getPixelColor(incident, attr_1);}
};


#endif //PATHTRACER_SYCL_TRIANGLE_H
