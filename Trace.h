////
//// Created by sysgen on 3/21/21.
////
//
//#ifndef PATHTRACER_SYCL_TRACE_H
//#define PATHTRACER_SYCL_TRACE_H
//
//#include <CL/sycl.hpp>
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
//#endif //PATHTRACER_SYCL_TRACE_H
