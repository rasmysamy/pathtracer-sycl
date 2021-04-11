//
// Created by sysgen on 3/20/21.
//

#include "Material.h"
//Ray material::reflectMat(const Ray &incident, const Ray &normal, sc::float3 albedo, int material, sc::float3 rand) {
//    rand = sc::normalize(rand);
//    if(sc::dot(rand, normal.getDirection()) < 0){
//        rand = -rand;
//    }
//    //Ray rRay = Ray(normal.getOrigin(), rand, albedo * incident.getLuminance());
////    Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.05), normal.getDirection(), albedo * incident.getLuminance());
//    Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.05), rand, albedo * incident.getLuminance());
//    //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.0005), sc::normalize(normal.getDirection()+(rand*0.7)), albedo * incident.getLuminance());
//    return rRay;
//}
//Ray material::reflectMat(const Ray& incident, const intersectReturn& i, sc::float3 rand) {
//    rand = sc::normalize(rand);
//    if(sc::dot(rand, i.normal) < 0){
//        rand = -rand;
//    }
//    //Ray rRay = Ray(normal.getOrigin(), rand, albedo * incident.getLuminance());
//    //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.05), normal.getDirection(), albedo * incident.getLuminance());
//    Ray rRay = Ray(i.hitpoint+(i.normal*0.05), rand, i.attr_1 * incident.getLuminance());
//    //Ray rRay = Ray(normal.getOrigin()+(normal.getDirection()*0.0005), sc::normalize(normal.getDirection()+(rand*0.7)), albedo * incident.getLuminance());
//    return rRay;
//}


