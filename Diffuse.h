////
//// Created by sysgen on 3/21/21.
////
//
//#ifndef PATHTRACER_SYCL_DIFFUSE_H
//#define PATHTRACER_SYCL_DIFFUSE_H
//
//
//#include "Material.h"
//#include "Random.h"
//
//class Diffuse : public Material {
//private:
//    sc::float3 albedo;
//public:
//    explicit Diffuse(const sc::float3 &reflectance): albedo(reflectance){}
//    const Ray reflect(const Ray& incident, const Ray& normal){
//        sc::float3 rand = sc::normalize(sc::float3({randfloat(), randfloat(), randfloat()}));
//        return Ray(normal.getOrigin(), rand, albedo*incident.getLuminance());
//    }
//    bool isEmissive() const {return false;}
//};
//
//
//#endif //PATHTRACER_SYCL_DIFFUSE_H
