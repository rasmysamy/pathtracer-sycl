//
// Created by sysgen on 3/20/21.
//

#ifndef PATHTRACER_SYCL_MATERIAL_H
#define PATHTRACER_SYCL_MATERIAL_H

#include "Ray.h"

#include "Material.h"
#include "Random.h"

#define NORMAL_RECTIFICATION_COEFFICIENT 0.00005f

namespace material {
    enum MATERIALS {
        Diffuse = 0, Emissive = 1, Glossy = 2, Glass = 3
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

        bool isEmissive() const{return material==MATERIALS::Emissive;}
        sc::float3 getPixelEmissive(const Ray& incident) const{
            return getPixelColor(incident, attr_1);
        }
        Ray diffuseReflect(const Ray& incident, sc::float3 rand) const{
            rand = sc::normalize(rand);
            if(sc::dot(rand, normal) < 0)
                rand = -rand;
            Ray rRay = Ray(hitpoint+(normal*NORMAL_RECTIFICATION_COEFFICIENT), rand, attr_1 * incident.getLuminance());
            return rRay;
        }
        Ray glossyReflect(const Ray& incident, sc::float3 rand) const{ // This calculates a glossy reflection, which is mirror reflection with some blur defined by roughness.
            rand = sc::normalize(rand);
            float normalL = sc::dot(-incident.getDirection(), normal);
            sc::float3 normalProj = normal * normalL;
            sc::float3 reflect = incident.getDirection() - (2*sc::dot(incident.getDirection(), normal)*normal);
            if(sc::dot(rand, reflect) < 0)
                rand = -rand;
            sc::float3 scatterDir = reflect + attr_2*rand;
            if(sc::dot(scatterDir, normal)<0)
                scatterDir=-scatterDir;
            Ray rRay = Ray(hitpoint+(normal*NORMAL_RECTIFICATION_COEFFICIENT), reflect + attr_2*rand, attr_1 * incident.getLuminance());
            return rRay;
        }
        Ray mirrorReflect(const Ray& incident) const{
            float normalL = sc::dot(-incident.getDirection(), normal);
            sc::float3 normalProj = normal * normalL;
            sc::float3 reflect = sc::normalize((incident.getDirection()) + 2*normalProj);
            Ray rRay = Ray(hitpoint+(normal*NORMAL_RECTIFICATION_COEFFICIENT), reflect, incident.getLuminance());
            return rRay;
        }
        float calcFresnelReflectance(const Ray& incident, float cosIncident, float incidentRayIndex,
                                            float mediaIndex) const{ //We apply Fresnel's equations to get mean unpolarized reflectance
            // We use Snell's law to find the sine of the exit angle
            float sinTheta2 = incidentRayIndex / mediaIndex * sc::sqrt(std::max(0.f, 1 - cosIncident * cosIncident));
            // Total internal reflection was already handled above.
            float cosTheta2 = sc::sqrt(std::max(0.f, 1 - sinTheta2 * sinTheta2));
            float rSPolarization = ((mediaIndex * cosIncident) - (incidentRayIndex * cosTheta2)) / ((mediaIndex * cosIncident) + (incidentRayIndex * cosTheta2));
            float rPPolarization = ((incidentRayIndex * cosIncident) - (mediaIndex * cosTheta2)) / ((incidentRayIndex * cosIncident) + (mediaIndex * cosTheta2));
            return (rSPolarization * rSPolarization + rPPolarization * rPPolarization) / 2; //We take the average of the reflectances for the two polarizations
        }
        Ray glass(const Ray& incident, int rand) const{ //This calculates glass media changes, which may reflect the light or let it pass through.
            auto n = normal;
            sc::float3 hitPointRectified = hitpoint + -n*NORMAL_RECTIFICATION_COEFFICIENT;
            float cosIncidentAngle = sc::clamp(sc::dot(incident.getDirection(), normal), -1.0f, 1.0f);
            float incidentRayIndex = 1, mediaIndex = attr_2;
            float iorRatio = incidentRayIndex / mediaIndex;
            float k = 1 - iorRatio * iorRatio * (1 - cosIncidentAngle * cosIncidentAngle);
            if (cosIncidentAngle < 0) {
                cosIncidentAngle = -cosIncidentAngle;
            } else {
                std::swap(incidentRayIndex, mediaIndex);
                n = -normal;
                hitPointRectified = hitpoint + -n*NORMAL_RECTIFICATION_COEFFICIENT;
            }
            float r = calcFresnelReflectance(incident, cosIncidentAngle, incidentRayIndex, mediaIndex);
            if(k<0 || (r*32768)>rand) {
                return mirrorReflect(incident);
            }
            sc::float3 refractDirection = iorRatio * incident.getDirection() + (iorRatio * cosIncidentAngle - sc::sqrt(k)) * n;
            return Ray(hitPointRectified, refractDirection, attr_1*incident.getLuminance() * (1-r));
        }
        Ray reflect(const Ray& incident, sc::float4 rand, int randNum) const{ //Dispatches the correct function based on ray type.
            if(material==MATERIALS::Glass)
                return glass(incident, randNum);
            sc::float3 rand3 = sc::float3(BoxMueller(rand).x(), BoxMueller(rand).y(), BoxMueller(rand).z());
            if(material==MATERIALS::Diffuse)
                return diffuseReflect(incident, rand3);
            return glossyReflect(incident, rand3);
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
