////
//// Created by sysgen on 3/21/21.
////
//
#ifndef PATHTRACER_SYCL_TRACE_H
#define PATHTRACER_SYCL_TRACE_H
//
#include <CL/sycl.hpp>
#include "Ray.h"
#include "Sphere.h"
#include "kdTreeMesh.h"

namespace sc = cl::sycl;

class pathtracing;

void trace(sc::handler &cgh, sc::buffer<Ray, 1> &rBuf, sc::buffer<Sphere, 1> &oBuf, sc::buffer<sc::float3, 1> &iBuf,
           sc::buffer<kdTreeMesh, 1> &kBuf, sc::buffer<int, 1> &sBuf, int width, int height,
           int reflections, int samples, sc::float3 skyColor, int step){

    auto r_acc = rBuf.get_access<sc::access::mode::read>(cgh);
    auto o_acc = oBuf.get_access<sc::access::mode::read>(cgh);
    auto i_acc = iBuf.get_access<sc::access::mode::write>(cgh);
    auto k_acc = kBuf.get_access<sc::access::mode::read>(cgh);
    auto s_acc = sBuf.get_access<sc::access::mode::read_write>(cgh);
        cgh.parallel_for<pathtracing>(sc::range<2>(width, height), [=](sc::id<2> idx) {
            int x = idx[0];
            int y = idx[1];
            Ray initialRay = r_acc[x + y * width];
            Ray r;
            bool intersect;
            int seed = s_acc[x + y * width];
            fastrand(&seed);
            fastrand(&seed);
            sc::float3 sum = {0, 0, 0};
            //We initialize values necessary for the path tracing code.
            for (int i = 0; i < step; i++) {
                sc::float3 color = {0, 0, 0};
                r = initialRay;

                material::intersectReturn tobj;

                material::intersectReturn lastIntersect = material::intersectReturn();
                for (int j = 0; j < reflections; j++) {
                    intersect = false;
                    float minDist = FLT_MAX;
                    for (int k = 0; k < o_acc.get_count(); k++) {
                        material::intersectReturn ret = o_acc[k].mRayIntersect(r, k);
                        if (ret == lastIntersect)
                            continue;
                        if (ret.intersect
                            && ret.intersectDistance < minDist) {
                            intersect = true;
                            minDist = ret.intersectDistance;
                            tobj = ret;
                        }
                    }
                    for (int k = 0; k < k_acc.get_count(); ++k) {
                        material::intersectReturn t_intersect = k_acc[k].mRayIntersect(r, lastIntersect);
                        if (t_intersect.intersect
                            && t_intersect.intersectDistance < minDist) {
                            intersect = true;
//                            hitMesh = true;
                            minDist = t_intersect.intersectDistance;
                            tobj = t_intersect;
                        }
                    }

                    if (intersect) {
                        if (tobj.isEmissive()) {
                            color = tobj.getPixelEmissive(r);
                            if (j == 0) {
                                sum = tobj.getPixelEmissive(r) * samples;
                                goto imageWrite;
                            }
                            break;
                        } else {
                            sc::float4 randVec = sc::normalize(
                                    sc::float4(fastrand(&seed), fastrand(&seed),
                                               fastrand(&seed), fastrand(&seed)));
                            r = tobj.reflect(r, randVec, fastrand(&seed));
                        }
                    } else {
                        color = r.getLuminance() * skyColor;
                        if (j == 0) {
                            sum = skyColor * step;
                            goto imageWrite;
                        }
                        if (r.getLuminance().x() <= 0.05 && r.getLuminance().y() <= 0.05 && r.getLuminance().z() <= 0.05) {
                            goto imageWrite;
                        }
                        break;
                    }
                    lastIntersect = tobj;
                }
                sum += color;
            }
            imageWrite:
//            auto prev = i_acc[x + y * width];
//            prev = {0,0,0};
            i_acc[x + y * width] += sum;
            s_acc[x + y * width] = seed;
        });

}
//
//namespace sc = cl::sycl;
//
//const int MAX_RECURSION_DEPTH = 5;
//
//std::tuple<Ray, sc::float3, bool, bool> traceSingleRay(const Ray& ray, sc::accessor<Sphere> sphere, const int objSize){
//    Ray reflect;
//    bool intersect = false;
//    float minDist = MAXFLOAT;
//    int intersectIndex = 0;
//    for(int i = 0; i<objSize; i++){
//        Ray t_reflect;
//        bool t_intersect;
//        std::tie(t_reflect, t_intersect) = sphere[i].rayIntersect(ray);
//        if(t_intersect) {
//            float dist = sc::fast_length(ray.getOrigin()-t_reflect.getOrigin());
//            bool smallest = minDist > dist;
//            if (smallest) {
//                minDist = dist;
//                reflect = t_reflect;
//                intersect = true;
//                intersectIndex = i;
//            }
//        }
//    }
//    if(intersect){
//        if(sphere[intersectIndex].isEmissive())
//            return std::tuple(reflect, sphere[intersectIndex].getPixelEmissive(ray), true, true);
//        return std::tuple(reflect, sc::float3({30,30,30}), true, false);
//    }
//    return std::tuple(reflect, sc::float3({30,30,30}), false, false);
//}
//
//sc::float3 trace(const Ray& ray, sc::accessor<Sphere> sphere, const int objSize) {
//    float minDist = MAXFLOAT;
//    Ray reflect;
//    sc::float3 returnColor;
//    bool intersect;
//    bool hitEmissive;
//    int intersectIndex;
//    std::tie(reflect, returnColor, intersect, hitEmissive) = traceSingleRay(ray, sphere, objSize);
//    for(int i = 1; i<MAX_RECURSION_DEPTH; i++){
//        if(hitEmissive || !intersect){
//            return returnColor;
//        }
//        std::tie(reflect, returnColor, intersect, hitEmissive) = traceSingleRay(reflect, sphere, objSize);
//        return{255,1,1};
//    }
//    return {0,0,0};
//}
//
//
//
//
#endif //PATHTRACER_SYCL_TRACE_H
