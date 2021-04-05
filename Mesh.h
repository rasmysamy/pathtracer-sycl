//
// Created by sysgen on 3/30/21.
//

#ifndef PATHTRACER_SYCL_MESH_H
#define PATHTRACER_SYCL_MESH_H


#include "Triangle.h"

class Mesh {
    int material;
    sc::float3 attr_1;
    sc::float3 bBoxCenter;
    sc::float3 bBoxDims;
    std::vector<Triangle> tris;
public:
    Mesh(std::vector<Triangle> tri, sc::float3 color, int mat):tris(tri), attr_1(color), material(mat){
        sc::float3 min, max;
        for(auto & t : tris){
            sc::float3 a, b, c;
            a = t.v0;
            b = t.v1;
            c = t.v2;
            //Now we set the maximums and minimums
            {
                max.x() = sc::max(max.x(), a.x());
                max.x() = sc::max(max.x(), b.x());
                max.x() = sc::max(max.x(), c.x());
                
                max.y() = sc::max(max.y(), a.y());
                max.y() = sc::max(max.y(), b.y());
                max.y() = sc::max(max.y(), c.y());
                
                max.z() = sc::max(max.z(), a.z());
                max.z() = sc::max(max.z(), b.z());
                max.z() = sc::max(max.z(), c.z());

                min.x() = sc::min(min.x(), a.x());
                min.x() = sc::min(min.x(), b.x());
                min.x() = sc::min(min.x(), c.x());

                min.y() = sc::min(min.y(), a.y());
                min.y() = sc::min(min.y(), b.y());
                min.y() = sc::min(min.y(), c.y());

                min.z() = sc::min(min.z(), a.z());
                min.z() = sc::min(min.z(), b.z());
                min.z() = sc::min(min.z(), c.z());
            }
        }
        // We now average maximums and minimums to find center, and substract min from max to get dimensions
        bBoxCenter = (min+max)/2;
        bBoxDims = max-min;
    }
    bool bBoxIntersect(const Ray &r){
        sc::float3 tmin =
    }
    material::intersectReturn rayIntersect(const Ray &r){

    }
};


#endif //PATHTRACER_SYCL_MESH_H
