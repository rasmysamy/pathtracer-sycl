////
//// Created by sysgen on 3/21/21.
////
//
//#ifndef PATHTRACER_SYCL_EMISSIVE_H
//#define PATHTRACER_SYCL_EMISSIVE_H
//
//
//#include "Material.h"
//
//class Emissive : public Material {
//private:
//    sc::float3 emissivity;
//public:
//    Emissive(const sc::float3 &color): emissivity(color) {};
//    bool isEmissive() const{return true;};
//    sc::float3 getPixelColor(const Ray &incident) const{
//        return incident.getLuminance()*emissivity;
//    }
//};
//
//
//#endif //PATHTRACER_SYCL_EMISSIVE_H
