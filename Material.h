//
// Created by sysgen on 3/20/21.
//

#ifndef PATHTRACER_SYCL_MATERIAL_H
#define PATHTRACER_SYCL_MATERIAL_H

#include "Ray.h"

namespace material {
    enum MATERIALS {
        Diffuse = 0, Emissive = 1
    };

    inline sc::float3 getPixelColor(const Ray &incident, sc::float3 emissivity) {
        return incident.getLuminance() * emissivity;
    }
    struct intersectReturn{
        sc::float3 normal = sc::float3({0,0,0});
        sc::float3 hitpoint = sc::float3({0,0,0});
        sc::float3 attr_1 = sc::float3({0,0,0});
        int material = 0;
        bool intersect = false;
        int objType;



        bool isEmissive() const{return material==MATERIALS::Emissive;};
        sc::float3 getPixelEmissive(const Ray& incident) const{
            return getPixelColor(incident, attr_1);
        };
        Ray reflect(const Ray& incident, sc::float3 rand) const{
            rand = sc::normalize(rand);
            if(sc::dot(rand, normal) < 0)
                rand = -rand;
            Ray rRay = Ray(hitpoint+(normal*0.05), rand, attr_1 * incident.getLuminance());
            return rRay;
        }
    };

    Ray reflectMat(const Ray &incident, const Ray &normal, sc::float3 albedo, int material, sc::float3 rand);
    Ray reflectMat(const Ray& incident, const intersectReturn& i, sc::float3 rand);
}

#endif //PATHTRACER_SYCL_MATERIAL_H
