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

    SYCL_EXTERNAL Ray reflectMat(const Ray &incident, const Ray &normal, sc::float3 albedo, int material, sc::float3 rand);

    inline sc::float3 getPixelColor(const Ray &incident, sc::float3 emissivity) {
        return incident.getLuminance() * emissivity;
    }
}

#endif //PATHTRACER_SYCL_MATERIAL_H
