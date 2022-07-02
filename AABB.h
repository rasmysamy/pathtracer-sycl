//
// Created by sysgen on 4/5/21.
//

#ifndef PATHTRACER_SYCL_AABB_H
#define PATHTRACER_SYCL_AABB_H

#include "Ray.h"
#include "Triangle.h"
#include "cfloat"

class AABB {
public:
    sc::float3 bounds[2];

    AABB(sc::float3 a, sc::float3 b){
        bounds[0] = a;
        bounds[1] = b;
    }

    AABB(){
        bounds[0] = {0,0,0};
        bounds[1] = {1,1,1};
    }


    static inline void swap(float &a, float &b){
        float temp = b;
        b=a;
        a=temp;
    }

    static float minComponent(sc::float3 a){
        return sc::min(a.x(), sc::min(a.y(), a.z()));
    }
    static float maxComponent(sc::float3 a){
        return sc::max(a.x(), sc::max(a.y(), a.z()));
    }

    bool bRayIntersect(const Ray& r) const{
        sc::float3 first = (bounds[0] - r.getOrigin()) * 1/r.getDirection();
        sc::float3 second = (bounds[1] - r.getOrigin()) * 1/r.getDirection();
        sc::float3 tMin = sc::min(first, second);
        sc::float3 tMax = sc::max(first, second);
        return maxComponent(tMin) <= minComponent(tMax);
    }

//    bool bRayIntersect(const Ray& r, sc::float3 start, sc::float3 end){
//        sc::float3 first = (start - r.getOrigin()) * 1/r.getDirection();
//        sc::float3 second = (end - r.getOrigin()) * 1/r.getDirection();
//        sc::float3 tMin = sc::min(first, second);
//        sc::float3 tMax = sc::max(first, second);
//        return maxComponent(tMin) <= minComponent(tMax);
//    }

    float fRayIntersect(const Ray& r) const{
        sc::float3 first = (bounds[0] - r.getOrigin()) * 1/r.getDirection();
        sc::float3 second = (bounds[1] - r.getOrigin()) * 1/r.getDirection();
        sc::float3 tMin = sc::min(first, second);
        sc::float3 tMax = sc::max(first, second);
        if (maxComponent(tMin) >= minComponent(tMax))
            return -1;
        return maxComponent(tMin);
    }

    inline void getVertices(sc::float3 (vertices)[8]) const{
        vertices[0] = (bounds[0]);
        vertices[1] = (bounds[0] + (bounds[1] * sc::float3(1,0,0)));
        vertices[2] = (bounds[0] + (bounds[1] * sc::float3(1,1,0)));
        vertices[3] = (bounds[0] + (bounds[1] * sc::float3(0,1,0)));
        vertices[4] = (bounds[0] + (bounds[1] * sc::float3(0,1,1)));
        vertices[5] = (bounds[0] + (bounds[1] * sc::float3(0,0,1)));
        vertices[6] = (bounds[0] + (bounds[1] * sc::float3(1,0,1)));
        vertices[7] = (bounds[1]);
    }

    bool bTriIntersect(const Triangle& t) const{
        sc::float3 aabbNormals[] = {{1,0,0},{0,1,0},{0,0,1}};
        sc::float3 vertices[8];
        getVertices(vertices);

        for(sc::float3 n : aabbNormals){
            float p0,p1,p2;
            p0 = (sc::dot(t.v0, n) / sc::dot(n,n));
            p1 = (sc::dot(t.v1, n) / sc::dot(n,n));
            p2 = (sc::dot(t.v2, n) / sc::dot(n,n));
            float min,max,start,end;
            min = sc::min(sc::min(p0, p1), p2);
            max = sc::max(sc::max(p0, p1), p2);
            start = sc::min(sc::dot(n, bounds[0]), sc::dot(n, bounds[1]));
            end = sc::max(sc::dot(n, bounds[0]), sc::dot(n, bounds[1]));
            if(max < start || min > end)
                return false;
        }

        float tDot = sc::dot(t.getNormal(), t.v0);
        float min = FLT_MAX;
        float max = FLT_MIN;

        for(sc::float3 v : vertices){
            max = sc::max(max, (sc::dot(v, t.getNormal()) / sc::dot(t.getNormal(),t.getNormal())));
            min = sc::min(min, (sc::dot(v, t.getNormal()) / sc::dot(t.getNormal(),t.getNormal())));
        }

        if(max < tDot || min > tDot)
            return false;

        sc::float3 triEdges[3] = {t.v0 - t.v2, t.v1 - t.v0, t.v2 - t.v1};

        for(sc::float3 n : aabbNormals){
            for(sc::float3 e : triEdges){
                sc::float3 cross = sc::cross(e, n);

                float bMin = FLT_MAX;
                float bMax = FLT_MIN;
                float tMin = FLT_MAX;
                float tMax = FLT_MIN;

                for(sc::float3 v : vertices){
                    bMax = sc::max(bMax, (sc::dot(v, cross) / sc::dot(cross,cross)));
                    bMin = sc::min(bMin, (sc::dot(v, cross) / sc::dot(cross,cross)));
                }

                float p0 = (sc::dot(t.v0, cross) / sc::dot(cross,cross));
                float p1 = (sc::dot(t.v1, cross) / sc::dot(cross,cross));
                float p2 = (sc::dot(t.v2, cross) / sc::dot(cross,cross));

                tMin = sc::min(sc::min(p0, p1), p2);
                tMax = sc::max(sc::max(p0, p1), p2);

                if(bMax <= tMin || bMin >= tMax)
                    return false;
            }
        }
        return true;
    }
    sc::float3 getSize() const{
        return bounds[1] - bounds[0];
    }
    sc::float3 getCenter() const{
        return (bounds[0]+bounds[1])/2;
    }
};


#endif //PATHTRACER_SYCL_AABB_H
