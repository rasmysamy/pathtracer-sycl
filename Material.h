//
// Created by sysgen on 3/20/21.
//

#ifndef PATHTRACER_SYCL_MATERIAL_H
#define PATHTRACER_SYCL_MATERIAL_H

#include "Ray.h"

#include "Material.h"

namespace material {
    enum MATERIALS {
        Diffuse = 0, Emissive = 1, Glossy = 2
    };

    inline sc::float3 getPixelColor(const Ray &incident, sc::float3 emissivity) {
        return incident.getLuminance() * emissivity;
    }
    class intersectReturn{
    public:
        sc::float3 normal = {0,0,0};
        sc::float3 hitpoint = {0,0,0};
        sc::float3 attr_1 = {50,50,50}; //Generally color, can also be emissivity
        float attr_2 = 0; //This is either index of refraction, or roughness.
        int material = 0;
        bool intersect = false;
        float intersectDistance = -1;
        int stackPos = -1;
        int objType = -1;



        bool isEmissive() const{return material==MATERIALS::Emissive;};
        sc::float3 getPixelEmissive(const Ray& incident) const{
            return getPixelColor(incident, attr_1);
        };
        Ray diffuseReflect(const Ray& incident, sc::float3 rand) const{
            rand = sc::normalize(rand);
            if(sc::dot(rand, normal) < 0)
                rand = -rand;
            Ray rRay = Ray(hitpoint+(normal*0.0005), rand, attr_1 * incident.getLuminance());
            return rRay;
        }
        Ray glossyReflect(const Ray& incident, sc::float3 rand) const{
            rand = sc::normalize(rand);
            float normalL = sc::dot(-incident.getDirection(), normal);
            sc::float3 normalProj = normal * normalL;
            sc::float3 reflect = sc::normalize((incident.getDirection()) + 2*normalProj);
            if(sc::dot(rand, reflect) < 0)
                rand = -rand;
            sc::float3 scatterDir = reflect + attr_2*rand;
            if(sc::dot(scatterDir, normal)<0)
                scatterDir=-scatterDir;
            Ray rRay = Ray(hitpoint+(normal*0.0005), reflect + attr_2*rand, attr_1 * incident.getLuminance());
            return rRay;
        }
        Ray mirrorReflect(const Ray& incident) const{
            float normalL = sc::dot(-incident.getDirection(), normal);
            sc::float3 normalProj = normal * normalL;
            sc::float3 reflect = sc::normalize((incident.getDirection()) + 2*normalProj);
            Ray rRay = Ray(hitpoint+(normal*0.0005), reflect, attr_1 * incident.getLuminance());
            return rRay;
        }
        Ray glassInto(const Ray& incident, float rand) {
            float cosIncidentAngle = sc::clamp(-1, 1, sc::dot(incident.getDirection(), normal));
            float incidentRayIndex = 1, mediaIndex = attr_1;
            float iorRatio = incidentRayIndex / mediaIndex;
            float k = 1 - eta * eta * (1 - cosi * cosi);
            if(k<0)
                return mirrorReflect(incident);
            sc::float3 refractDirection = eta * incident.getDirection + (eta * cosIncidentAngle - sc::sqrt(k)) * n;
        }
        Ray reflect(const Ray& incident, sc::float3 rand) const{
            if(material==MATERIALS::Diffuse)
                return diffuseReflect(incident, rand);
            return glossyReflect(incident, rand);
        }
        bool operator==(const intersectReturn& rhs)const{
            return (this->stackPos == rhs.stackPos && this->objType == rhs.objType);
        }
        bool operator!=(const intersectReturn& rhs)const{
            return !(*this==rhs);
        }
    };

    inline Ray reflectMat(const Ray &incident, const Ray &normal, sc::float3 albedo, int material, sc::float3 rand)
    {
        rand = sc::normalize(rand);
        if(sc::dot(rand, normal.getDirection()) < 0){
            rand = -rand;
        }
        //Ray rRay = Ray(normal.getOrigin(), rand, albedo * incident.getLuminance());
//    Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.05), normal.getDirection(), albedo * incident.getLuminance());
        Ray rRay = Ray(normal.getOrigin(), rand, albedo * incident.getLuminance());
        //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.0005), sc::normalize(normal.getDirection()+(rand*0.7)), albedo * incident.getLuminance());
        return rRay;
    }
//    Ray reflectMat(const Ray& incident, const intersectReturn& i, sc::float3 rand)
//    {
//        rand = sc::normalize(rand);
//        if(sc::dot(rand, i.normal) < 0){
//            rand = -rand;
//        }
//        //Ray rRay = Ray(normal.getOrigin(), rand, albedo * incident.getLuminance());
//        //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.05), normal.getDirection(), albedo * incident.getLuminance());
//        Ray rRay = Ray(i.hitpoint+(i.normal*0.05), rand, i.attr_1 * incident.getLuminance());
//        //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.0005), sc::normalize(normal.getDirection()+(rand*0.7)), albedo * incident.getLuminance());
//        return rRay;
//    }
}

#endif //PATHTRACER_SYCL_MATERIAL_H
