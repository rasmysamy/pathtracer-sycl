//
// Created by sysgen on 3/20/21.
//
#pragma once
#ifndef PATHTRACER_SYCL_RANDOM_H
#define PATHTRACER_SYCL_RANDOM_H

#include <limits>
#include <CL/sycl.hpp>

const float PI = 3.14156;

namespace sc = cl::sycl;

struct RandomResult
{
    sc::uint4 state;
    float value;
};

//SYCL_EXTERNAL RandomResult Random(sc::uint4 state);
inline int fastrand(int *g_seed) {
    *g_seed = (214013*(*g_seed)+2531011);
    return (*g_seed>>16)&0x7FFF;
}

inline float fastfrand(int *g_seed) {
    return (float)(fastrand(g_seed)%32768) / 32767.0f;
}

inline sc::float4 BoxMueller(sc::float4 uniformRandomUnit){
    float a = uniformRandomUnit.x();
    float b = uniformRandomUnit.y();
    float c = uniformRandomUnit.z();
    float d = uniformRandomUnit.w();
    return sc::float4({sc::sqrt(-2*sc::log(a))*sc::cos(2*PI*b),
                         sc::sqrt(-2*sc::log(a))*sc::sin(2*PI*b),
                         sc::sqrt(-2*sc::log(c))*sc::cos(2*PI*d),
                         sc::sqrt(-2*sc::log(c))*sc::sin(2*PI*d)});
}

inline sc::float3 randVec(float t, float a){
    float theta = t*2*PI;
    a = a*2 - 1;
    float b = sc::sqrt(1-a*a);
    float x = sc::cos(theta)*b;
    float y = sc::sin(theta)*b;
    float z = a;
    return sc::float3({x, y, z});
}

//inline sc::float3 BoxMueller3(sc::float4 uniformRandomUnit){
//    float a = uniformRandomUnit.x();
//    float b = uniformRandomUnit.y();
//    float c = uniformRandomUnit.z();
//    float d = uniformRandomUnit.w();
//    return sc::float3({sc::sqrt(-2*sc::log(a))*sc::cos(2*PI*b),
//                         sc::sqrt(-2*sc::log(a))*sc::sin(2*PI*b),
//                         sc::sqrt(-2*sc::log(c))*sc::cos(2*PI*d)});
//}


#endif //PATHTRACER_SYCL_RANDOM_H
